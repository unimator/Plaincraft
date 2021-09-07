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
#include <stdexcept>
#include <iostream>
#include <chrono>
#include "texture/vulkan_textures_factory.hpp"
#include "shader/vulkan_shader.hpp"
#include "renderer/vertex_utils.hpp"
#include "window/vulkan_window.hpp"
#include "utils/queue_family.hpp"
#include "swapchain/swapchain.hpp"

const std::vector<plaincraft_render_engine::Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4};

namespace plaincraft_render_engine_vulkan
{
	VulkanRenderEngine::VulkanRenderEngine(std::shared_ptr<VulkanWindow> window) : VulkanRenderEngine::VulkanRenderEngine(std::move(window), false) {}

	VulkanRenderEngine::VulkanRenderEngine(std::shared_ptr<VulkanWindow> window, bool enable_debug)
		: RenderEngine(std::move(window)),
		  enable_debug_(enable_debug),
		  instance_(VulkanInstance(VulkanInstanceConfig())),
		  surface_(GetVulkanWindow()->CreateSurface(instance_.GetInstance())),
		  device_(VulkanDevice(instance_, surface_)),
		  swapchain_(Swapchain(GetVulkanWindow(), device_, surface_)),
		  buffer_manager_(VulkanBufferManager(device_)),
		  image_manager_(VulkanImageManager(device_))
	{
		vertex_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\vert.spv");
		fragment_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\frag.spv");
		
		VulkanPipelineConfig pipeline_config{};
		CreateDescriptorSetLayout();
		CreatePipelineLayout();
		VkViewport viewport{};
		VkRect2D scissor{};
		SetupPipelineConfig(pipeline_config, viewport, scissor);

		LoadModel();

		pipeline_ = std::make_unique<VulkanPipeline>(device_, vertex_shader_code_, fragment_shader_code_, pipeline_config);
		
		auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\text.png");
		image_manager_.CreateTextureImage(image, texture_image_, texture_image_memory_);
		image_manager_.CreateTextureImageView(texture_image_, texture_image_view_);
		image_manager_.CreateTextureSampler(texture_sampler_);
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		CreateSyncObjects();

		textures_factory_ = std::make_shared<VulkanTexturesFactory>(VulkanTexturesFactory());
	}

	VulkanRenderEngine::~VulkanRenderEngine()
	{
		CleanupSwapChain();

		vkDestroySampler(device_.GetDevice(), texture_sampler_, nullptr);
		vkDestroyImageView(device_.GetDevice(), texture_image_view_, nullptr);
		vkDestroyImage(device_.GetDevice(), texture_image_, nullptr);
		vkFreeMemory(device_.GetDevice(), texture_image_memory_, nullptr);

		vkDestroyBuffer(device_.GetDevice(), vertex_buffer_, nullptr);
		vkFreeMemory(device_.GetDevice(), vertex_buffer_memory_, nullptr);

		vkDestroyBuffer(device_.GetDevice(), index_buffer_, nullptr);
		vkFreeMemory(device_.GetDevice(), index_buffer_memory_, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(device_.GetDevice(), render_finished_semaphores_[i], nullptr);
			vkDestroySemaphore(device_.GetDevice(), image_available_semaphores_[i], nullptr);
			vkDestroyFence(device_.GetDevice(), in_flight_fences_[i], nullptr);
		}

		vkDestroySurfaceKHR(instance_.GetInstance(), surface_, nullptr);

		vkDestroyDescriptorSetLayout(device_.GetDevice(), descriptor_set_layout_, nullptr);
		vkDestroyPipelineLayout(device_.GetDevice(), pipeline_layout_, nullptr);
	}

	void VulkanRenderEngine::CleanupSwapChain()
	{
		vkFreeCommandBuffers(device_.GetDevice(), device_.GetCommandPool(), static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
		
		for (size_t i = 0; i < swapchain_.GetSwapchainImages().size(); ++i)
		{
			vkDestroyBuffer(device_.GetDevice(), uniform_buffers_[i], nullptr);
			vkFreeMemory(device_.GetDevice(), uniform_buffers_memory_[i], nullptr);
		}

		swapchain_.CleanupSwapchain();

		vkDestroyDescriptorPool(device_.GetDevice(), descriptor_pool_, nullptr);
	}

	void VulkanRenderEngine::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding mvp_layout_binding{};
		mvp_layout_binding.binding = 0;
		mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		mvp_layout_binding.descriptorCount = 1;
		mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		mvp_layout_binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding sampler_layout_binding{};
		sampler_layout_binding.binding = 1;
		sampler_layout_binding.descriptorCount = 1;
		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding.pImmutableSamplers = nullptr;
		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {mvp_layout_binding, sampler_layout_binding};
		VkDescriptorSetLayoutCreateInfo layout_info{};
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
		layout_info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device_.GetDevice(), &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout");
		}
	}

	void VulkanRenderEngine::CreatePipelineLayout() 
	{
		VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout_;
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		pipeline_layout_create_info.pPushConstantRanges = nullptr;
		pipeline_layout_create_info.pNext = VK_NULL_HANDLE;

		if(vkCreatePipelineLayout(device_.GetDevice(), &pipeline_layout_create_info, nullptr, &pipeline_layout_) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void VulkanRenderEngine::SetupPipelineConfig(VulkanPipelineConfig& pipeline_config, VkViewport& viewport, VkRect2D& scissor)
	{
		pipeline_config.descriptor_set_layout = descriptor_set_layout_;
		pipeline_config.pipeline_layout = pipeline_layout_;
		pipeline_config.render_pass = swapchain_.GetRenderPass();

		VulkanPipeline::CreateDefaultPipelineConfig(pipeline_config);		
		auto swapchain_extent = swapchain_.GetSwapchainExtent();
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain_extent.width);
		viewport.height = static_cast<float>(swapchain_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = {0, 0};
		scissor.extent = swapchain_extent;
		pipeline_config.viewport_info.pViewports = &viewport;
		pipeline_config.viewport_info.pScissors = &scissor;
	}

	void VulkanRenderEngine::CreateCommandBuffers()
	{
		command_buffers_.resize(swapchain_.GetSwapchainFrameBuffers().size());

		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.commandPool = device_.GetCommandPool();
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

		if (vkAllocateCommandBuffers(device_.GetDevice(), &allocate_info, command_buffers_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (size_t i = 0; i < command_buffers_.size(); ++i)
		{
			VkCommandBufferBeginInfo buffer_begin_info{};
			buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			buffer_begin_info.flags = 0;
			buffer_begin_info.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(command_buffers_[i], &buffer_begin_info) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = swapchain_.GetRenderPass();
			render_pass_info.framebuffer = swapchain_.GetSwapchainFrameBuffers()[i];
			render_pass_info.renderArea.offset = {0, 0};
			render_pass_info.renderArea.extent = swapchain_.GetSwapchainExtent();

			std::array<VkClearValue, 2> clear_colors{};
			clear_colors[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
			clear_colors[1].depthStencil = {1.0f, 0};
			render_pass_info.clearValueCount = static_cast<uint32_t>(clear_colors.size());
			render_pass_info.pClearValues = clear_colors.data();

			vkCmdBeginRenderPass(command_buffers_[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->GetPipeline());

			VkBuffer vertex_buffers[] = {vertex_buffer_};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(command_buffers_[i], 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(command_buffers_[i], index_buffer_, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1, &descriptor_sets_[i], 0, nullptr);
			//vkCmdDraw(command_buffers_[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
			//vkCmdDrawIndexed(command_buffers_[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
			model_->Bind(command_buffers_[i]);
			model_->Draw(command_buffers_[i]);
			vkCmdEndRenderPass(command_buffers_[i]);

			if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer");
			}
		}
	}

	void VulkanRenderEngine::CreateSyncObjects()
	{
		image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
		images_in_flight_.resize(swapchain_.GetSwapchainImages().size(), VK_NULL_HANDLE);

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

		CleanupSwapChain();

		swapchain_.RecreateSwapchain();

		VulkanPipelineConfig pipeline_config{};
		VkViewport viewport{};
		VkRect2D scissor{};
		SetupPipelineConfig(pipeline_config, viewport, scissor);
		pipeline_ = std::make_unique<VulkanPipeline>(device_, vertex_shader_code_, fragment_shader_code_, pipeline_config);
		
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
	}

	void VulkanRenderEngine::CreateVertexBuffer()
	{
		VkDeviceSize buffer_size = sizeof(Vertex) * vertices.size();

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		buffer_manager_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void *data;
		vkMapMemory(device_.GetDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, vertices.data(), buffer_size);
		vkUnmapMemory(device_.GetDevice(), staging_buffer_memory);

		buffer_manager_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_, vertex_buffer_memory_);

		buffer_manager_.CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);

		vkDestroyBuffer(device_.GetDevice(), staging_buffer, nullptr);
		vkFreeMemory(device_.GetDevice(), staging_buffer_memory, nullptr);
	}

	void VulkanRenderEngine::CreateIndexBuffer()
	{
		VkDeviceSize buffer_size = sizeof(uint16_t) * indices.size();

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		buffer_manager_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void *data;
		vkMapMemory(device_.GetDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, indices.data(), buffer_size);
		vkUnmapMemory(device_.GetDevice(), staging_buffer_memory);

		buffer_manager_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_, index_buffer_memory_);
		buffer_manager_.CopyBuffer(staging_buffer, index_buffer_, buffer_size);

		vkDestroyBuffer(device_.GetDevice(), staging_buffer, nullptr);
		vkFreeMemory(device_.GetDevice(), staging_buffer_memory, nullptr);
	}

	void VulkanRenderEngine::CreateUniformBuffers()
	{
		VkDeviceSize buffer_size = sizeof(plaincraft_render_engine::ModelViewProjectionMatrix);

		auto swapchain_images = swapchain_.GetSwapchainImages();
		uniform_buffers_.resize(swapchain_images.size());
		uniform_buffers_memory_.resize(swapchain_images.size());

		for (size_t i = 0; i < swapchain_images.size(); ++i)
		{
			buffer_manager_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_[i], uniform_buffers_memory_[i]);
		}
	}

	void VulkanRenderEngine::CreateDescriptorPool()
	{
		auto swapchain_images = swapchain_.GetSwapchainImages();
		std::array<VkDescriptorPoolSize, 2> pool_sizes{};
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = static_cast<uint32_t>(swapchain_images.size());
		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = static_cast<uint32_t>(swapchain_images.size());

		VkDescriptorPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		pool_info.pPoolSizes = pool_sizes.data();
		pool_info.maxSets = static_cast<uint32_t>(swapchain_images.size());

		if (vkCreateDescriptorPool(device_.GetDevice(), &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
	}

	void VulkanRenderEngine::CreateDescriptorSets()
	{
		auto swapchain_images = swapchain_.GetSwapchainImages();
		std::vector<VkDescriptorSetLayout> layouts(swapchain_images.size(), descriptor_set_layout_);
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
		descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_allocate_info.descriptorPool = descriptor_pool_;
		descriptor_set_allocate_info.descriptorSetCount = static_cast<uint32_t>(swapchain_images.size());
		descriptor_set_allocate_info.pSetLayouts = layouts.data();

		descriptor_sets_.resize(swapchain_images.size());
		if (vkAllocateDescriptorSets(device_.GetDevice(), &descriptor_set_allocate_info, descriptor_sets_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor sets");
		}

		for (size_t i = 0; i < swapchain_images.size(); ++i)
		{
			VkDescriptorBufferInfo descriptor_buffer_info{};
			descriptor_buffer_info.buffer = uniform_buffers_[i];
			descriptor_buffer_info.offset = 0;
			descriptor_buffer_info.range = sizeof(ModelViewProjectionMatrix);

			VkDescriptorImageInfo descriptor_image_info{};
			descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptor_image_info.imageView = texture_image_view_;
			descriptor_image_info.sampler = texture_sampler_;

			std::array<VkWriteDescriptorSet, 2> write_descriptor_sets{};
			write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_descriptor_sets[0].dstSet = descriptor_sets_[i];
			write_descriptor_sets[0].dstBinding = 0;
			write_descriptor_sets[0].dstArrayElement = 0;
			write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write_descriptor_sets[0].descriptorCount = 1;
			write_descriptor_sets[0].pBufferInfo = &descriptor_buffer_info;
			write_descriptor_sets[0].pImageInfo = nullptr;
			write_descriptor_sets[0].pTexelBufferView = nullptr;

			write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_descriptor_sets[1].dstSet = descriptor_sets_[i];
			write_descriptor_sets[1].dstBinding = 1;
			write_descriptor_sets[1].dstArrayElement = 0;
			write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write_descriptor_sets[1].descriptorCount = 1;
			write_descriptor_sets[1].pBufferInfo = nullptr;
			write_descriptor_sets[1].pImageInfo = &descriptor_image_info;
			write_descriptor_sets[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(device_.GetDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
		}
	}

	void VulkanRenderEngine::UpdateUniformBuffer(uint32_t image_index)
	{
		static auto start_time = std::chrono::high_resolution_clock::now();

		auto current_time = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

		auto swapchain_extent = swapchain_.GetSwapchainExtent();
		ModelViewProjectionMatrix mvp_matrix{};
		mvp_matrix.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp_matrix.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp_matrix.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 10.0f);
		mvp_matrix.projection[1][1] *= -1;

		void *data;
		vkMapMemory(device_.GetDevice(), uniform_buffers_memory_[image_index], 0, sizeof(mvp_matrix), 0, &data);
		memcpy(data, &mvp_matrix, sizeof(mvp_matrix));
		vkUnmapMemory(device_.GetDevice(), uniform_buffers_memory_[image_index]);
	}

	void VulkanRenderEngine::CreateDepthResources()
	{
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
		VkResult result = vkAcquireNextImageKHR(device_.GetDevice(), swapchain_.GetSwapchain(), UINT64_MAX, image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		if (images_in_flight_[image_index] != VK_NULL_HANDLE)
		{
			vkWaitForFences(device_.GetDevice(), 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
		}

		images_in_flight_[image_index] = in_flight_fences_[current_frame_];

		UpdateUniformBuffer(image_index);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = {image_available_semaphores_[current_frame_]};
		VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffers_[image_index];

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

		VkSwapchainKHR swap_chains[] = {swapchain_.GetSwapchain()};
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

	void VulkanRenderEngine::LoadModel()
	{
		auto polygon = std::make_shared<const Cube>(Cube());
		model_ = std::make_unique<VulkanModel>(device_, polygon);
	}
}