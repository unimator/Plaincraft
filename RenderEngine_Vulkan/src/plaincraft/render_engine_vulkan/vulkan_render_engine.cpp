/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Górka

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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "vulkan_render_engine.hpp"
#include "textures/vulkan_textures_factory.hpp"
#include "shader/vulkan_shader.hpp"
#include "rendering/scene/vertex_utils.hpp"
#include "window/vulkan_window.hpp"
#include "utils/queue_family.hpp"
#include "swapchain/vulkan_swapchain.hpp"
#include "rendering/scene/vulkan_scene_renderer.hpp"
#include "rendering/vulkan_renderer_frame_config.hpp"
#include "models/vulkan_models_factory.hpp"
#include <stdexcept>
#include <iostream>
#include <glm/gtx/quaternion.hpp>

namespace plaincraft_render_engine_vulkan
{
	VulkanRenderEngine::VulkanRenderEngine(std::shared_ptr<VulkanWindow> window) : VulkanRenderEngine::VulkanRenderEngine(std::move(window), false) {}

	VulkanRenderEngine::VulkanRenderEngine(std::shared_ptr<VulkanWindow> window, bool enable_debug)
		: RenderEngine(std::move(window)),
		  enable_debug_(enable_debug),
		  instance_(VulkanInstance(VulkanInstanceConfig())),
		  surface_(GetVulkanWindow()->CreateSurface(instance_.GetInstance())),
		  device_(VulkanDevice(instance_, surface_))
	{
		models_factory_ = std::make_unique<VulkanModelsFactory>(VulkanModelsFactory(device_));

		RecreateSwapChain();

		CreateSyncObjects();

		textures_factory_ = std::make_shared<VulkanTexturesFactory>(device_);
	}

	VulkanRenderEngine::~VulkanRenderEngine()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(device_.GetDevice(), render_finished_semaphores_[i], nullptr);
			vkDestroySemaphore(device_.GetDevice(), image_available_semaphores_[i], nullptr);
			vkDestroyFence(device_.GetDevice(), in_flight_fences_[i], nullptr);
		}

		if (gui_renderer_ != nullptr)
		{
			gui_renderer_.reset();
		}

		if (swapchain_ != nullptr)
		{
			swapchain_.reset(); // swapchain has to be destroyed before surface is released
		}

		vkDestroySurfaceKHR(instance_.GetInstance(), surface_, nullptr);

		scene_renderer_.reset(); // because it relies on device which would be deleted before renderer
	}

	void VulkanRenderEngine::CreateCommandBuffers()
	{
		command_buffers_.resize(swapchain_->GetSwapchainFrameBuffers().size());

		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.commandPool = device_.GetCommandPool();
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

		if (vkAllocateCommandBuffers(device_.GetDevice(), &allocate_info, command_buffers_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void VulkanRenderEngine::CreateSyncObjects()
	{
		image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
		images_in_flight_.resize(swapchain_->GetSwapchainImages().size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphore_info{};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info{};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS || vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS || vkCreateFence(device_.GetDevice(), &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame");
			}
		}
	}

	void VulkanRenderEngine::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window_->GetInstance(), &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window_->GetInstance(), &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device_.GetDevice());

		if (swapchain_ == nullptr)
		{
			swapchain_ = std::make_unique<Swapchain>(GetVulkanWindow(), device_, surface_);
		}
		else
		{
			swapchain_ = std::make_unique<Swapchain>(GetVulkanWindow(), device_, surface_, std::move(swapchain_));
		}

		scene_renderer_ = std::make_unique<VulkanSceneRenderer>(device_, swapchain_->GetRenderPass(), swapchain_->GetSwapchainExtent(), swapchain_->GetSwapchainImages().size(), camera_);

		if (gui_renderer_ == nullptr)
		{
			gui_renderer_ = std::make_unique<VulkanGuiRenderer>(instance_, device_, GetVulkanWindow(), swapchain_->GetRenderPass());
		}
		else
		{
			gui_renderer_ = std::make_unique<VulkanGuiRenderer>(instance_, device_, GetVulkanWindow(), swapchain_->GetRenderPass(), std::move(gui_renderer_));
		}

		CreateCommandBuffers();
	}

	uint32_t VulkanRenderEngine::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_properties)
	{
		VkPhysicalDeviceMemoryProperties device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(device_.GetPhysicalDevice(), &device_memory_properties);

		for (uint32_t i = 0; i < device_memory_properties.memoryTypeCount; ++i)
		{
			if ((type_filter & (1 << i)) && (device_memory_properties.memoryTypes[i].propertyFlags & memory_properties) == memory_properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

	void VulkanRenderEngine::RenderFrame(const FrameConfig &frame_config)
	{
		vkWaitForFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

		glfwPollEvents();
		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(device_.GetDevice(), swapchain_->GetSwapchain(), UINT64_MAX, image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		auto command_buffer = GetCommandBuffer(current_frame_);

		if (images_in_flight_[image_index] != VK_NULL_HANDLE)
		{
			vkWaitForFences(device_.GetDevice(), 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
		}

		VkCommandBufferBeginInfo command_buffer_begin_info{};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		images_in_flight_[image_index] = in_flight_fences_[current_frame_];

		VkRenderPassBeginInfo render_pass_begin_info{};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = swapchain_->GetRenderPass();
		render_pass_begin_info.framebuffer = swapchain_->GetSwapchainFrameBuffers()[image_index];
		render_pass_begin_info.renderArea.offset = {0, 0};
		render_pass_begin_info.renderArea.extent = swapchain_->GetSwapchainExtent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
		clear_values[1].depthStencil = {1.0f, 0};
		render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_begin_info.pClearValues = clear_values.data();

		auto vulkan_renderer = GetVulkanRenderer();

		VulkanRendererFrameConfig vulkan_renderer_frame_config{
			frame_config,
			command_buffer,
			image_index};

		vulkan_renderer->BeginFrame(vulkan_renderer_frame_config);

		for (auto i = 0; i < drawables_list_.size(); ++i)
		{
			auto &drawable = drawables_list_[i];
			scene_renderer_->Batch(drawable);
		}

		vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		vulkan_renderer->Render();
		vulkan_renderer->EndFrame();
		vulkan_renderer->HasRendered();

		gui_renderer_->Render(vulkan_renderer_frame_config);

		vkCmdEndRenderPass(command_buffer);

		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = {image_available_semaphores_[current_frame_]};
		VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		VkSemaphore signal_semaphores[] = {render_finished_semaphores_[current_frame_]};
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		vkResetFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_]);
		if (vkQueueSubmit(device_.GetGraphicsQueue(), 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submitt draw command buffer");
		}

		VkPresentInfoKHR presentation_info{};
		presentation_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentation_info.waitSemaphoreCount = 1;
		presentation_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swap_chains[] = {swapchain_->GetSwapchain()};
		presentation_info.swapchainCount = 1;
		presentation_info.pSwapchains = swap_chains;
		presentation_info.pImageIndices = &image_index;
		presentation_info.pResults = nullptr;

		auto presentation_queue = device_.GetPresentationQueue();
		result = vkQueuePresentKHR(presentation_queue, &presentation_info);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || GetVulkanWindow()->WasResized())
		{
			GetVulkanWindow()->ResetWasResized();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		vkQueueWaitIdle(presentation_queue);

		current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
	}
}