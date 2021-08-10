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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_SWAPCHAIN
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_SWAPCHAIN

#include "../device/vulkan_device.hpp"
#include "../utils/queue_family.hpp"
#include "../window/vulkan_window.hpp"
#include <plaincraft_render_engine.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace plaincraft_render_engine_vulkan
{
    using namespace plaincraft_render_engine;

    class Swapchain final {
    private:
        const VulkanDevice& device_;
        const VkSurfaceKHR& surface_;
        std::shared_ptr<VulkanWindow> window_;

        VkSwapchainKHR swapchain_;
		VkFormat swapchain_image_format_;
		VkExtent2D swapchain_extent_;

        std::vector<VkImage> swapchain_images_;
		std::vector<VkImageView> swapchain_images_views_;
		std::vector<VkFramebuffer> swapchain_frame_buffers_;

    public:
        Swapchain(std::shared_ptr<VulkanWindow> window, const VulkanDevice& device, const VkSurfaceKHR& surface);

        Swapchain(const Swapchain& other) = delete;
        Swapchain(Swapchain&& other);

        Swapchain& operator=(const Swapchain& other) = delete;
        Swapchain& operator=(Swapchain&& other);

        auto GetSwapchain() const -> VkSwapchainKHR { return swapchain_; }
        auto GetSwapchainImageFormat() const -> VkFormat { return swapchain_image_format_; }
        auto GetSwapchainExtent() const -> VkExtent2D { return swapchain_extent_; }

        auto GetSwapchainImages() -> std::vector<VkImage>& { return swapchain_images_; }
        auto GetSwapchainImagesViews() -> std::vector<VkImageView>& { return swapchain_images_views_; }
        auto GetSwapchainFrameBuffers() -> std::vector<VkFramebuffer>& { return swapchain_frame_buffers_; }

        void RecreateSwapchain();

    private:
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        void CreateImageViews();
    };

    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    SwapchainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR& surface);
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_SWAPCHAIN

