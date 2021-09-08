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

#include "vulkan_renderer.hpp"
#include <chrono>
#include <glm\gtx\quaternion.hpp>

namespace plaincraft_render_engine_vulkan {
    VulkanRenderer::VulkanRenderer(const VulkanDevice& device, Swapchain& swapchain, std::shared_ptr<Camera> camera, VkImageView texture_view, VkSampler texture_sampler) 
    :
        Renderer(camera),
        device_(device),
        swapchain_(swapchain),
        buffer_manager_(VulkanBufferManager(device)),
        texture_image_view_(texture_view),
        texture_sampler_(texture_sampler)
    {
		vertex_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\vert.spv");
		fragment_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\frag.spv");

        LoadModel();

        VulkanPipelineConfig pipeline_config{};
		CreateDescriptorSetLayout();
		CreatePipelineLayout();
		VkViewport viewport{};
		VkRect2D scissor{};
		SetupPipelineConfig(pipeline_config, viewport, scissor);

		pipeline_ = std::make_unique<VulkanPipeline>(device_, vertex_shader_code_, fragment_shader_code_, pipeline_config);

        CreateUniformBuffers();
        CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
    }

	void VulkanRenderer::LoadModel()
	{
		auto polygon = std::make_shared<const Cube>(Cube());
		model_ = std::make_unique<VulkanModel>(device_, polygon);
	}

    VulkanRenderer::~VulkanRenderer()
    {
		vkFreeCommandBuffers(device_.GetDevice(), device_.GetCommandPool(), static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
		
		for (size_t i = 0; i < uniform_buffers_.size(); ++i)
		{
			vkDestroyBuffer(device_.GetDevice(), uniform_buffers_[i], nullptr);
			vkFreeMemory(device_.GetDevice(), uniform_buffers_memory_[i], nullptr);
		}

		vkDestroyDescriptorSetLayout(device_.GetDevice(), descriptor_set_layout_, nullptr);
		vkDestroyDescriptorPool(device_.GetDevice(), descriptor_pool_, nullptr);
		vkDestroyPipelineLayout(device_.GetDevice(), pipeline_layout_, nullptr);
    }

    void VulkanRenderer::Render() 
    {
		CreateCommandBuffers();
		drawables_list_.clear();
    }

    void VulkanRenderer::CreateCommandBuffers()
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

			vkCmdBindDescriptorSets(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1, &descriptor_sets_[i], 0, nullptr);
			//vkCmdDraw(command_buffers_[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
			//vkCmdDrawIndexed(command_buffers_[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

			// for(auto drawable : drawables_list_) 
			// {
			// 	auto modelObject = drawable->GetModel();
			// 	auto polygon = modelObject->GetPolygon();
			// 	const auto scale = drawable->GetScale();
			// 	const auto position = drawable->GetPosition();
			// 	const auto rotation = drawable->GetRotation();

			// 	glm::mat4 projection = glm::perspective(glm::radians(camera_->fov), (float)1024 / (float)768, 0.1f, 100.0f);
			// 	glm::mat4 view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);
			// 	auto model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), Vector3d(scale, scale, scale)) * glm::toMat4(rotation);

			// 	ModelViewProjectionMatrix mvp_matrix{};
			// 	mvp_matrix.model = model;
			// 	mvp_matrix.view = view;
			// 	mvp_matrix.projection = projection;
			// 	mvp_matrix.projection[1][1] *= -1;

			// 	void *data;
			// 	vkMapMemory(device_.GetDevice(), uniform_buffers_memory_[i], 0, sizeof(mvp_matrix), 0, &data);
			// 	memcpy(data, &mvp_matrix, sizeof(mvp_matrix));
			// 	vkUnmapMemory(device_.GetDevice(), uniform_buffers_memory_[i]);

			// 	model_->Bind(command_buffers_[i]);
			// 	model_->Draw(command_buffers_[i]);
			// }
			model_->Bind(command_buffers_[i]);
			model_->Draw(command_buffers_[i]);
			vkCmdEndRenderPass(command_buffers_[i]);

			if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer");
			}
		}
    }

    void VulkanRenderer::UpdateUniformBuffer(uint32_t image_index)
	{
		static auto start_time = std::chrono::high_resolution_clock::now();

		auto current_time = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

		auto swapchain_extent = swapchain_.GetSwapchainExtent();
		// ModelViewProjectionMatrix mvp_matrix{};

		// mvp_matrix.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		// mvp_matrix.view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);
		// mvp_matrix.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 10.0f);
		// mvp_matrix.projection[1][1] *= -1;

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
    
	void VulkanRenderer::SetupPipelineConfig(VulkanPipelineConfig& pipeline_config, VkViewport& viewport, VkRect2D& scissor)
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

    void VulkanRenderer::CreateDescriptorSetLayout()
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

    void VulkanRenderer::CreateDescriptorPool()
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

    void VulkanRenderer::CreateDescriptorSets()
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

	void VulkanRenderer::CreatePipelineLayout() 
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

	void VulkanRenderer::CreateUniformBuffers()
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
}