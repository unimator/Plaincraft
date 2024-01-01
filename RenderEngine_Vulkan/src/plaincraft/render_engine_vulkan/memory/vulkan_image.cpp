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

#include "vulkan_image.hpp"
#include <stdexcept>

namespace plaincraft_render_engine_vulkan
{
    VulkanImage::VulkanImage(const VulkanDevice &device,
                             uint32_t width,
                             uint32_t height,
                             VkFormat format,
                             VkImageTiling image_tiling,
                             VkImageUsageFlags image_usage_flags,
                             VkMemoryPropertyFlags memory_property_flags)
        : device_(device),
          width_(width),
          height_(height),
          format_(format),
          image_tiling_(image_tiling),
          image_usage_flags_(image_usage_flags),
          memory_property_flags_(memory_property_flags)
    {
        CreateImage();
    }

    VulkanImage::VulkanImage(VulkanImage &&other)
        : device_(other.device_),
          width_(other.width_),
          height_(other.height_),
          format_(other.format_),
          image_tiling_(other.image_tiling_),
          image_usage_flags_(other.image_usage_flags_),
          memory_property_flags_(other.memory_property_flags_)

    {
        other.image_ = VK_NULL_HANDLE;
        other.image_memory_ = VK_NULL_HANDLE;
    }

    VulkanImage &VulkanImage::operator=(VulkanImage &&other)
    {
        if (this == &other)
        {
            return *this;
        }

        this->image_ = other.image_;
        this->format_ = other.format_;
        this->image_memory_ = other.image_memory_;
        this->width_ = other.width_;
        this->height_ = other.height_;
        this->image_tiling_ = other.image_tiling_;
        this->memory_property_flags_ = other.memory_property_flags_;

        other.image_ = VK_NULL_HANDLE;
        other.image_memory_ = VK_NULL_HANDLE;
        other.format_ = VK_FORMAT_UNDEFINED;
        other.width_ = 0;
        other.height_ = 0;
        other.image_tiling_ = VK_IMAGE_TILING_OPTIMAL;
        other.memory_property_flags_ = 0;

        return *this;
    }

    VulkanImage::~VulkanImage()
    {
        auto &device = device_.get();
        vkDestroyImage(device.GetDevice(), image_, nullptr);

        // if(image_memory_ != VK_NULL_HANDLE)
        // {
        //     vkFreeMemory(device.GetDevice(), image_memory_, nullptr);
        // }
    }

    VkImage VulkanImage::GetImage() const
    {
        return image_;
    }

    VkDeviceMemory VulkanImage::GetImageMemory() const
    {
        return image_memory_;
    }

    uint32_t VulkanImage::GetWidth() const
    {
        return width_;
    }

    uint32_t VulkanImage::GetHeight() const
    {
        return height_;
    }

    VkFormat VulkanImage::GetFormat() const
    {
        return format_;
    }

    void VulkanImage::CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height)
    {
        auto &device = device_.get();
        auto command_buffer = device.BeginSingleTimeCommands(device.GetTransferCommandPool());

        VkBufferImageCopy image_copy_region{};
        image_copy_region.bufferOffset = 0;
        image_copy_region.bufferRowLength = 0;
        image_copy_region.bufferImageHeight = 0;

        image_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_copy_region.imageSubresource.mipLevel = 0;
        image_copy_region.imageSubresource.baseArrayLayer = 0;
        image_copy_region.imageSubresource.layerCount = 1;

        image_copy_region.imageOffset = {0, 0, 0};
        image_copy_region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(command_buffer, buffer, image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_copy_region);

        device.EndSingleTimeCommands(device.GetTransferCommandPool(), command_buffer, device.GetTransferQueue());
    }

    void VulkanImage::CreateImage()
    {
        auto &device = device_.get();

        VkImageCreateInfo image_create_info{};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = width_;
        image_create_info.extent.height = height_;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.format = format_;
        image_create_info.tiling = image_tiling_;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = image_usage_flags_;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.flags = 0;

        auto vk_device = device.GetDevice();

        auto result = vkCreateImage(vk_device, &image_create_info, nullptr, &image_);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image");
        }

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(vk_device, image_, &memory_requirements);

        VkMemoryAllocateInfo memory_allocate_info{};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = device.FindMemoryType(memory_requirements.memoryTypeBits, memory_property_flags_);

        if (vkAllocateMemory(vk_device, &memory_allocate_info, nullptr, &image_memory_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image memory");
        }

        vkBindImageMemory(vk_device, image_, image_memory_, 0);
    }
}