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
#include "texture/vulkan_textures_factory.hpp"
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
		auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\text.png");
		texture_image_ = std::make_unique<VulkanTexture>(device_, image);
		texture_image_view_ = std::make_unique<VulkanImageView>(device_, texture_image_->GetImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

		models_factory_ = std::make_unique<VulkanModelsFactory>(VulkanModelsFactory(device_));

		RecreateSwapChain();

		CreateSyncObjects();

		textures_factory_ = std::make_shared<VulkanTexturesFactory>(VulkanTexturesFactory());
	}

	VulkanRenderEngine::~VulkanRenderEngine()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(device_.GetDevice(), render_finished_semaphores_[i], nullptr);
			vkDestroySemaphore(device_.GetDevice(), image_available_semaphores_[i], nullptr);
			vkDestroyFence(device_.GetDevice(), in_flight_fences_[i], nullptr);
		}

		if(gui_renderer_ != nullptr)
		{
			gui_renderer_.reset();
		}

		if(swapchain_ != nullptr)
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

		descriptor_set_layout_ = VulkanDescriptorSetLayout::Builder(device_)
									 .AddLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 1)
									 .AddLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
									 .Build();

		scene_renderer_ = std::make_unique<VulkanSceneRenderer>(device_, swapchain_->GetRenderPass(), swapchain_->GetSwapchainExtent(), descriptor_set_layout_->GetDescriptorSetLayout(), camera_);

		auto images_count = swapchain_->GetSwapchainImages().size();
		descriptor_pool_ = VulkanDescriptorPool::Builder(device_)
							   .SetMaxSets(images_count)
							   .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1)
							   .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
							   .Build();

		if(gui_renderer_ == nullptr) 
		{
			gui_renderer_ = std::make_unique<VulkanGuiRenderer>(instance_, device_, GetVulkanWindow(), swapchain_->GetRenderPass());
		}
		else
		{
			gui_renderer_ = std::make_unique<VulkanGuiRenderer>(instance_, device_, GetVulkanWindow(), swapchain_->GetRenderPass(), std::move(gui_renderer_));
		}

		CreateUniformBuffers();
		CreateDescriptors();
		CreateCommandBuffers();
	}

	void VulkanRenderEngine::CreateDescriptors()
	{
		auto images_count = swapchain_->GetSwapchainImages().size();
		descriptor_sets_.resize(images_count);
		for (size_t i = 0; i < descriptor_sets_.size(); ++i)
		{
			VkDescriptorBufferInfo descriptor_buffer_info{};
			descriptor_buffer_info.buffer = uniform_buffers_[i]->GetBuffer();
			descriptor_buffer_info.offset = 0;
			descriptor_buffer_info.range = sizeof(ModelViewProjectionMatrix);

			VkDescriptorImageInfo descriptor_image_info{};
			descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptor_image_info.imageView = texture_image_view_->GetImageView();
			descriptor_image_info.sampler = texture_image_->GetSampler();

			VulkanDescriptorWriter descriptor_writer(*descriptor_set_layout_, *descriptor_pool_);
			descriptor_writer.WriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, &descriptor_buffer_info);
			descriptor_writer.WriteImage(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &descriptor_image_info);
			descriptor_writer.Build(descriptor_sets_[i]);
		}
	}

	void VulkanRenderEngine::CreateUniformBuffers()
	{
		VkPhysicalDeviceProperties physical_device_properties;
		vkGetPhysicalDeviceProperties(device_.GetPhysicalDevice(), &physical_device_properties);
		auto min_ubo_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;

		auto swapchain_images = swapchain_->GetSwapchainImages();
		uniform_buffers_.resize(swapchain_images.size());

		uint32_t instance_count = (40 * 40 + 1);

		for (size_t i = 0; i < swapchain_images.size(); ++i)
		{
			uniform_buffers_[i] = std::make_unique<VulkanBuffer>(device_, sizeof(ModelViewProjectionMatrix), instance_count, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, min_ubo_alignment);
		}
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

	void VulkanRenderEngine::RenderFrame()
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

		// vulkan_renderer->UpdateUniformBuffer(image_index);

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

		vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		auto vulkan_renderer = GetVulkanRenderer();

		glm::mat4 projection = glm::perspective(glm::radians(camera_->fov), (float)1024 / (float)768, 0.1f, 100.0f);
		projection[1][1] *= -1;
		glm::mat4 view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);
		auto &uniform_buffer = uniform_buffers_[image_index];
		auto alignment_size = uniform_buffer->GetAlignmentSize();

		for (auto i = 0; i < drawables_list_.size(); ++i)
		{
			auto drawable = drawables_list_[i];
			scene_renderer_->Batch(drawable);
			auto color = drawable->GetColor();
			const auto scale = drawable->GetScale();
			const auto position = drawable->GetPosition();
			const auto rotation = drawable->GetRotation();
			auto model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), Vector3d(scale, scale, scale)) * glm::toMat4(rotation);

			plaincraft_render_engine::ModelViewProjectionMatrix mvp{
				model,
				view,
				projection,
				color};

			auto offset = static_cast<uint32_t>(i) * alignment_size;
			uniform_buffer->Map(alignment_size, offset);
			uniform_buffer->Write(&mvp, alignment_size, 0);
			uniform_buffer->Unmap();
		}

		VulkanRendererFrameConfig frame_config{
			command_buffer,
			descriptor_sets_[image_index],
			alignment_size};
		
		vulkan_renderer->Render(frame_config);
		vulkan_renderer->HasRendered();

		gui_renderer_->Render(frame_config);

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