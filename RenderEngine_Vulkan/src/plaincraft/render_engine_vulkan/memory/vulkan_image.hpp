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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE

#include "../device/vulkan_device.hpp"
#include <vulkan/vulkan.h>
#include <functional>

namespace plaincraft_render_engine_vulkan
{
    class VulkanImage
    {
    protected:
        std::reference_wrapper<const VulkanDevice> device_;

        VkImage image_;
        VkDeviceMemory image_memory_;
        VkFormat format_;
        VkImageTiling image_tiling_;
        VkImageUsageFlags image_usage_flags_;
        VkMemoryPropertyFlags memory_property_flags_;

        uint32_t width_;
        uint32_t height_;

    public:
        VulkanImage(const VulkanDevice& device, 
            uint32_t width, 
            uint32_t height, 
            VkFormat format, 
            VkImageTiling image_tiling, 
            VkImageUsageFlags image_usage_flags, 
            VkMemoryPropertyFlags memory_property_flags
            );

        VulkanImage(const VulkanImage& other) = delete;
        VulkanImage& operator=(const VulkanImage& other) = delete;

        VulkanImage(VulkanImage&& other);
        VulkanImage& operator=(VulkanImage&& other); 

        virtual ~VulkanImage();

        void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);

        auto GetImage() -> VkImage { return image_; }
        auto GetImageMemory() -> VkDeviceMemory { return image_memory_; }
        auto GetWidth() -> uint32_t { return width_; }
        auto GetHeight() -> uint32_t { return height_; }

    protected:
        void CreateImage();
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE