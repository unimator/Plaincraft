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

#include "swapchain.hpp"
#include "../utils/image_utils.hpp"

namespace plaincraft_render_engine_vulkan 
{
    Swapchain::Swapchain(std::shared_ptr<VulkanWindow> window, const VulkanDevice& device, const VkSurfaceKHR& surface)
		: window_(window), device_(device), surface_(surface)
    {
        RecreateSwapchain();
		CreateRenderPass();
		CreateFrameBuffers();
    }

	Swapchain::Swapchain(Swapchain&& other) 
		: swapchain_(other.swapchain_), window_(other.window_), device_(other.device_), surface_(other.surface_)
	{
		other.swapchain_ = VK_NULL_HANDLE;

		this->swapchain_image_format_ = other.swapchain_image_format_;
		other.swapchain_image_format_ = VK_FORMAT_UNDEFINED;

		this->swapchain_extent_ = other.swapchain_extent_;
	}

	void Swapchain::CleanupSwapchain()
	{
		for (auto framebuffer : swapchain_frame_buffers_)
		{
			vkDestroyFramebuffer(device_.GetDevice(), framebuffer, nullptr);
		}

		for (auto image_view : swapchain_images_views_)
		{
			vkDestroyImageView(device_.GetDevice(), image_view, nullptr);
		}

		vkDestroySwapchainKHR(device_.GetDevice(), swapchain_, nullptr);
	}

    void Swapchain::RecreateSwapchain()
    {
		auto swapchain_support = QuerySwapChainSupport(device_.GetPhysicalDevice(), surface_);

		auto surface_format = ChooseSwapSurfaceFormat(swapchain_support.formats);
		auto present_mode = ChooseSwapPresentMode(swapchain_support.present_modes);
		auto extent = ChooseSwapExtent(swapchain_support.capabilities);

		auto image_count = swapchain_support.capabilities.minImageCount + 1;
		if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount)
		{
			image_count = swapchain_support.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface_;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		auto indices = FindQueueFamilyIndices(device_.GetPhysicalDevice(), surface_);
		uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

		if (indices.graphics_family != indices.present_family)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}
		create_info.preTransform = swapchain_support.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device_.GetDevice(), &create_info, nullptr, &swapchain_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swapchain");
		}

		vkGetSwapchainImagesKHR(device_.GetDevice(), swapchain_, &image_count, nullptr);
		swapchain_images_.resize(image_count);
		vkGetSwapchainImagesKHR(device_.GetDevice(), swapchain_, &image_count, swapchain_images_.data());

		swapchain_image_format_ = surface_format.format;
		swapchain_extent_ = extent;

		CreateImageViews();
		CreateFrameBuffers();
    }

    VkSurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats)
	{
		for (const auto &available_format : available_formats)
		{
			if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return available_format;
			}
		}

		return available_formats[0];
	}

	
	VkPresentModeKHR Swapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes)
	{
		for (const auto &available_present_mode : available_present_modes)
		{
			if (available_present_mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return available_present_mode;
			}
		}

		return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(window_->GetInstance(), &width, &height);

			VkExtent2D actual_extent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
			actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

			return actual_extent;
		}
	}

	void Swapchain::CreateImageViews()
	{
		swapchain_images_views_.resize(swapchain_images_.size());

		for (size_t i = 0; i < swapchain_images_.size(); ++i)
		{
			swapchain_images_views_[i] = CreateImageView(device_.GetDevice(), swapchain_images_[i], swapchain_image_format_);
		}
	}

	void Swapchain::CreateRenderPass()
    {
        VkAttachmentDescription color_attachment{};
		color_attachment.format = swapchain_image_format_;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_reference{};
		color_attachment_reference.attachment = 0;
		color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass_description{};
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &color_attachment_reference;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass_description;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		if (vkCreateRenderPass(device_.GetDevice(), &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass");
		}
    }

	void Swapchain::CreateFrameBuffers()
	{
		swapchain_frame_buffers_.resize(swapchain_images_.size());
		auto swapchain_extent = swapchain_extent_;

		for (size_t i = 0; i < swapchain_images_views_.size(); ++i)
		{
			VkImageView attachments[] = {
				swapchain_images_views_[i]};

			VkFramebufferCreateInfo framebuffer_info{};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.renderPass = render_pass_;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = attachments;
			framebuffer_info.width = swapchain_extent_.width;
			framebuffer_info.height = swapchain_extent_.height;
			framebuffer_info.layers = 1;

			if (vkCreateFramebuffer(device_.GetDevice(), &framebuffer_info, nullptr, &swapchain_frame_buffers_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer");
			}
		}
	}

	SwapchainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR& surface)
	{
		SwapchainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
		if (format_count != 0)
		{
			details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
		if (present_mode_count != 0)
		{
			details.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
		}

		return details;
	}
}