/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Gorka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "vulkan_buffer.hpp"
#include <stdexcept>

namespace plaincraft_render_engine_vulkan
{
    VulkanBuffer::VulkanBuffer(const VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties, VkDeviceSize min_offset_alignment)
        : VulkanBuffer(device, size, 1, usage_flags, memory_properties, min_offset_alignment)
    {
    }

    VulkanBuffer::VulkanBuffer(const VulkanDevice &device, VkDeviceSize instance_size, uint32_t instance_count, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties, VkDeviceSize min_offset_alignment)
        : device_(device),
          instance_size_(instance_size),
          instance_count_(instance_count),
          memory_property_flags_(memory_properties),
          buffer_usage_flags_(usage_flags)
    {
        alignment_size_ = GetAlignment(min_offset_alignment);
        buffer_size_ = alignment_size_ * instance_count_;
        CreateBuffer();
    }

    VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept
        : device_(other.device_),
          buffer_(other.buffer_),
          memory_(other.memory_),
          buffer_size_(other.buffer_size_),
          instance_count_(other.instance_count_),
          instance_size_(other.instance_size_),
          alignment_size_(other.alignment_size_),
          buffer_usage_flags_(other.buffer_usage_flags_),
          memory_property_flags_(other.memory_property_flags_)
    {
        other.buffer_ = VK_NULL_HANDLE;
        other.memory_ = VK_NULL_HANDLE;
        other.buffer_size_ = 0;
        other.instance_count_ = 0;
        other.instance_size_ = 0;
        other.alignment_size_ = 0;
        other.buffer_usage_flags_ = 0;
        other.memory_property_flags_ = 0;
    }

    VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        this->device_ = other.device_;

        this->buffer_ = other.buffer_;
        this->memory_ = other.memory_;
        this->buffer_size_ = other.buffer_size_;
        this->instance_count_ = other.instance_count_;
        this->instance_size_ = other.instance_size_;
        this->alignment_size_ = other.alignment_size_;
        this->buffer_usage_flags_ = other.buffer_usage_flags_;
        this->memory_property_flags_ = other.memory_property_flags_;

        other.buffer_ = VK_NULL_HANDLE;
        other.memory_ = VK_NULL_HANDLE;
        other.buffer_size_ = 0;
        other.instance_count_ = 0;
        other.instance_size_ = 0;
        other.alignment_size_ = 0;
        other.buffer_usage_flags_ = 0;
        other.memory_property_flags_ = 0;

        return *this;
    }

    VulkanBuffer::~VulkanBuffer()
    {
        vkDestroyBuffer(device_.get().GetDevice(), buffer_, nullptr);
        vkFreeMemory(device_.get().GetDevice(), memory_, nullptr);
    }

    VkResult VulkanBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
    {
        if (size == VK_WHOLE_SIZE)
        {
            return vkMapMemory(device_.get().GetDevice(), memory_, 0, buffer_size_, 0, &mapped_data_);
        }
        return vkMapMemory(device_.get().GetDevice(), memory_, offset, size, 0, &mapped_data_);
    }

    void VulkanBuffer::Unmap()
    {
        if (mapped_data_)
        {
            vkUnmapMemory(device_.get().GetDevice(), memory_);
            mapped_data_ = nullptr;
        }
    }

    void VulkanBuffer::Write(void *data, VkDeviceSize size, VkDeviceSize offset)
    {
        if (size == VK_WHOLE_SIZE)
        {
            memcpy(mapped_data_, data, static_cast<size_t>(size));
        }
        else
        {
            char *mapped_data_offset_ = static_cast<char *>(mapped_data_) + offset;
            memcpy(mapped_data_offset_, data, static_cast<size_t>(size));
        }
    }

    void VulkanBuffer::CopyFromBuffer(const VulkanBuffer &other)
    {
        auto &device = device_.get();
        auto command_buffer = device.BeginSingleTimeCommands(device.GetTransferCommandPool());

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size = other.GetBufferSize();
        vkCmdCopyBuffer(command_buffer, other.buffer_, buffer_, 1, &copy_region);

        device.EndSingleTimeCommands(device.GetTransferCommandPool(), command_buffer, device.GetTransferQueue());
    }

    VulkanBuffer VulkanBuffer::MoveBuffer(const VulkanDevice &device, VulkanBuffer &&buffer, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties)
    {
        VulkanBuffer result(device, buffer.GetInstanceSize(), buffer.GetInstanceCount(), buffer_usage_flags, memory_properties);

        result.CopyFromBuffer(buffer);

        return result;
    }

    VkResult VulkanBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mapped_memory_range{};
        mapped_memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_memory_range.memory = memory_;
        mapped_memory_range.offset = offset;
        mapped_memory_range.size = size;
        return vkFlushMappedMemoryRanges(device_.get().GetDevice(), 1, &mapped_memory_range);
    }

    void VulkanBuffer::CreateBuffer()
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = buffer_size_;
        buffer_info.usage = buffer_usage_flags_;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto device = device_.get().GetDevice();

        if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer");
        }

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(device, buffer_, &memory_requirements);

        VkMemoryAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocate_info.allocationSize = memory_requirements.size;
        allocate_info.memoryTypeIndex = device_.get().FindMemoryType(memory_requirements.memoryTypeBits, memory_property_flags_);

        if (vkAllocateMemory(device, &allocate_info, nullptr, &memory_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate vertex buffer memory");
        }

        vkBindBufferMemory(device, buffer_, memory_, 0);
    }

    VkDeviceSize VulkanBuffer::GetAlignment(VkDeviceSize min_offset_alignment)
    {
        if (min_offset_alignment > 0)
        {
            return (instance_size_ + min_offset_alignment - 1) & ~(min_offset_alignment - 1);
        }
        return instance_size_;
    }
}