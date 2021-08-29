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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE_MANAGER
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE_MANAGER

#include "vulkan_buffer_manager.hpp"
#include <plaincraft_render_engine.hpp>
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan {
    using namespace plaincraft_render_engine;

    class VulkanImageManager {
    private:
        const VulkanDevice& device_;
        VulkanBufferManager buffer_manager_;

    public:
        VulkanImageManager(const VulkanDevice& device);

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling image_tiling, VkImageUsageFlags image_usage_flags, VkMemoryPropertyFlags memory_property_flags, VkImage& image, VkDeviceMemory& image_memory);
        void CreateImageView(VkImage image, VkFormat format, VkImageView& image_view);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        
		void CreateTextureImage(const Image& image, VkImage& texture_image, VkDeviceMemory& texture_image_memory);
		void CreateTextureImageView(VkImage texture_image, VkImageView& texture_image_view);
		void CreateTextureSampler(VkSampler& texture_sampler);
    private:
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_image_layout, VkImageLayout new_image_layout);
    };
}

#endif //PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE_MANAGER