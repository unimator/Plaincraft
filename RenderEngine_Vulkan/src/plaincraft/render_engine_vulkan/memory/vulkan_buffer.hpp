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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_BUFFER
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_BUFFER

#include "../device/vulkan_device.hpp"
#include <vulkan/vulkan.h>
#include <functional>

namespace plaincraft_render_engine_vulkan
{
    class VulkanBuffer
    {
    private:
        std::reference_wrapper<const VulkanDevice> device_;
        VkBuffer buffer_;
        VkDeviceMemory memory_;
        VkDeviceSize buffer_size_;
        uint32_t instance_count_;
        VkDeviceSize instance_size_;
        VkDeviceSize alignment_size_;
        VkBufferUsageFlags buffer_usage_flags_;
        VkMemoryPropertyFlags memory_property_flags_;

        void* mapped_data_ = nullptr;

    public:
        VulkanBuffer(const VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties, VkDeviceSize min_offset_alignment = 1);
        VulkanBuffer(const VulkanDevice &device, VkDeviceSize instance_size, uint32_t instance_count, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties, VkDeviceSize min_offset_alignment = 1);

        VulkanBuffer(const VulkanBuffer& other) = delete;
        VulkanBuffer& operator=(const VulkanBuffer& other) = delete;

        VulkanBuffer(VulkanBuffer&& other) noexcept;
        VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

        virtual ~VulkanBuffer();

        VkResult Map(VkDeviceSize size, VkDeviceSize offset);
        void Unmap();
        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void Write(void* data, VkDeviceSize size, VkDeviceSize offset);

        auto GetBuffer() -> VkBuffer { return buffer_; }
        auto GetMemory() -> VkDeviceMemory { return memory_; }
        auto GetBufferSize() const -> VkDeviceSize { return buffer_size_; }
        auto GetInstanceCount() -> uint32_t { return instance_count_; }
        auto GetInstanceSize() -> VkDeviceSize { return instance_size_; }
        auto GetAlignmentSize() -> VkDeviceSize { return alignment_size_; }
        auto GetBufferUsageFlags() -> VkBufferUsageFlags { return buffer_usage_flags_; }
        auto GetMemoryPropertyFlags() -> VkMemoryPropertyFlags { return memory_property_flags_; }
        auto GetMappedData() -> void* { return mapped_data_; }

        void CopyFromBuffer(const VulkanBuffer& other);

        template<typename T>
        static VulkanBuffer CreateFromVector(const VulkanDevice& device, std::vector<T> data, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties);

        static VulkanBuffer MoveBuffer(const VulkanDevice& device, VulkanBuffer&& buffer, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties);

    private:
        void CreateBuffer();

        VkDeviceSize GetAlignment(VkDeviceSize min_offset_alignment);
    };

    template<typename T>
    VulkanBuffer VulkanBuffer::CreateFromVector(const VulkanDevice& device, std::vector<T> data, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties)
    {
        auto instance_count = static_cast<uint32_t>(data.size());

        VulkanBuffer result(device, sizeof(T), instance_count, buffer_usage_flags, memory_properties);

        result.Map(VK_WHOLE_SIZE, 0);
        result.Write(data.data(), result.GetBufferSize(), 0);
        result.Unmap();

        return result;
    }
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_BUFFER