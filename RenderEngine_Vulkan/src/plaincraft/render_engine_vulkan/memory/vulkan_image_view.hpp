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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE_VIEW
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE_VIEW

#include "../device/vulkan_device.hpp"
#include <vulkan/vulkan.hpp>
#include <functional>

namespace plaincraft_render_engine_vulkan {
    class VulkanImageView {
    private:
        std::reference_wrapper<const VulkanDevice> device_;

        VkImageView image_view_;
        VkFormat format_;
        VkImageAspectFlags aspect_flags_;

    public:
        VulkanImageView(const VulkanDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

        VulkanImageView(const VulkanImageView& other) = delete;
        VulkanImageView& operator=(const VulkanImageView& other) = delete;

        VulkanImageView(VulkanImageView&& other) noexcept;
        VulkanImageView& operator=(VulkanImageView&& other) noexcept;

        ~VulkanImageView();

        auto GetImageView() -> VkImageView { return image_view_; }

    private:
        void CreateImageView(VkImage image);
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IMAGE_VIEW