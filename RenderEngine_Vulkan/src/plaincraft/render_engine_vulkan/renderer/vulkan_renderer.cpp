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
#include <fstream>
#include <glm\gtx\quaternion.hpp>

namespace plaincraft_render_engine_vulkan
{
	VulkanRenderer::VulkanRenderer(const VulkanDevice &device, VkRenderPass render_pass, VkExtent2D extent, VkDescriptorSetLayout descriptor_set_layout, std::shared_ptr<Camera> camera)
		: Renderer(camera),
		  device_(device),
		  render_pass_(render_pass),
		  extent_(extent),
		  descriptor_set_layout_(descriptor_set_layout)
	{
		vertex_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\vert.spv");
		fragment_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\frag.spv");

		VulkanPipelineConfig pipeline_config{};
		CreatePipelineLayout();
		VkViewport viewport{};
		VkRect2D scissor{};
		SetupPipelineConfig(pipeline_config, viewport, scissor);

		LoadModel();

		pipeline_ = std::make_unique<VulkanPipeline>(device_, vertex_shader_code_, fragment_shader_code_, pipeline_config);
	}

	void VulkanRenderer::LoadModel()
	{
		auto cube_model = read_file_raw("F:/Projekty/Plaincraft/Models/cube_half.obj");
		auto mesh = Mesh::LoadWavefront(cube_model.data());

		model_ = std::make_unique<VulkanModel>(device_, std::move(mesh));
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkDestroyPipelineLayout(device_.GetDevice(), pipeline_layout_, nullptr);
	}

	void VulkanRenderer::Render(VulkanRendererFrameConfig& frame_config)
	{
		auto command_buffer = frame_config.command_buffer;
		auto descriptor_set = frame_config.descriptor_set;
		pipeline_->Bind(command_buffer);
		glm::mat4 projection = glm::perspective(glm::radians(camera_->fov), (float)1024 / (float)768, 0.1f, 100.0f);
		projection[1][1] *= -1;
		glm::mat4 view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);

		for (auto i = 0; i < drawables_list_.size(); ++i)
		{
			auto drawable = drawables_list_[i];
			/*auto model_object = drawable->GetModel();
			auto polygon = modelObject->GetMesh();
			const auto scale = drawable->GetScale();
			const auto position = drawable->GetPosition();
			const auto rotation = drawable->GetRotation();

			SimplePushConstants push{};
			auto model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), Vector3d(scale, scale, scale)) * glm::toMat4(rotation);
			push.transform = projection * view * model;
			vkCmdPushConstants(
				command_buffer,
				pipeline_layout_,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstants),
				&push);*/
				
			uint32_t dynamic_offset = i * frame_config.d;
			vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1, &descriptor_set, 1, &dynamic_offset);
			model_->Bind(command_buffer);
			model_->Draw(command_buffer);
		}
	}

	void VulkanRenderer::CreatePipelineLayout()
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(SimplePushConstants);

		VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout_;
		pipeline_layout_create_info.pushConstantRangeCount = 1;
		pipeline_layout_create_info.pPushConstantRanges = &push_constant_range;
		pipeline_layout_create_info.pNext = VK_NULL_HANDLE;

		if (vkCreatePipelineLayout(device_.GetDevice(), &pipeline_layout_create_info, nullptr, &pipeline_layout_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void VulkanRenderer::SetupPipelineConfig(VulkanPipelineConfig &pipeline_config, VkViewport &viewport, VkRect2D &scissor)
	{
		pipeline_config.descriptor_set_layout = descriptor_set_layout_;
		pipeline_config.pipeline_layout = pipeline_layout_;
		pipeline_config.render_pass = render_pass_;

		VulkanPipeline::CreateDefaultPipelineConfig(pipeline_config);
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(extent_.width);
		viewport.height = static_cast<float>(extent_.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = {0, 0};
		scissor.extent = extent_;
		pipeline_config.viewport_info.pViewports = &viewport;
		pipeline_config.viewport_info.pScissors = &scissor;
	}

	// void VulkanRenderer::CreateDescriptorSetLayout()
	// {
	// 	VkDescriptorSetLayoutBinding mvp_layout_binding{};
	// 	mvp_layout_binding.binding = 0;
	// 	mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// 	mvp_layout_binding.descriptorCount = 1;
	// 	mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	// 	mvp_layout_binding.pImmutableSamplers = nullptr;

	// 	VkDescriptorSetLayoutBinding sampler_layout_binding{};
	// 	sampler_layout_binding.binding = 1;
	// 	sampler_layout_binding.descriptorCount = 1;
	// 	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// 	sampler_layout_binding.pImmutableSamplers = nullptr;
	// 	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	// 	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {mvp_layout_binding, sampler_layout_binding};
	// 	VkDescriptorSetLayoutCreateInfo layout_info{};
	// 	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	// 	layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
	// 	layout_info.pBindings = bindings.data();

	// 	if (vkCreateDescriptorSetLayout(device_.GetDevice(), &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS)
	// 	{
	// 		throw std::runtime_error("Failed to create descriptor set layout");
	// 	}
	// }

	// void VulkanRenderer::CreateDescriptorPool()
	// {
	// 	auto swapchain_images = swapchain_.GetSwapchainImages();
	// 	std::array<VkDescriptorPoolSize, 2> pool_sizes{};
	// 	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// 	pool_sizes[0].descriptorCount = static_cast<uint32_t>(swapchain_images.size());
	// 	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// 	pool_sizes[1].descriptorCount = static_cast<uint32_t>(swapchain_images.size());

	// 	VkDescriptorPoolCreateInfo pool_info{};
	// 	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// 	pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	// 	pool_info.pPoolSizes = pool_sizes.data();
	// 	pool_info.maxSets = static_cast<uint32_t>(swapchain_images.size());

	// 	if (vkCreateDescriptorPool(device_.GetDevice(), &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
	// 	{
	// 		throw std::runtime_error("Failed to create shader module");
	// 	}
	// }

	// void VulkanRenderer::CreateDescriptorSets()
	// {
	// 	auto swapchain_images = swapchain_.GetSwapchainImages();
	// 	std::vector<VkDescriptorSetLayout> layouts(swapchain_images.size(), descriptor_set_layout_);
	// 	VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
	// 	descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	// 	descriptor_set_allocate_info.descriptorPool = descriptor_pool_;
	// 	descriptor_set_allocate_info.descriptorSetCount = static_cast<uint32_t>(swapchain_images.size());
	// 	descriptor_set_allocate_info.pSetLayouts = layouts.data();

	// 	descriptor_sets_.resize(swapchain_images.size());
	// 	if (vkAllocateDescriptorSets(device_.GetDevice(), &descriptor_set_allocate_info, descriptor_sets_.data()) != VK_SUCCESS)
	// 	{
	// 		throw std::runtime_error("Failed to allocate descriptor sets");
	// 	}

	// 	for (size_t i = 0; i < swapchain_images.size(); ++i)
	// 	{
	// 		VkDescriptorBufferInfo descriptor_buffer_info{};
	// 		descriptor_buffer_info.buffer = uniform_buffers_[i];
	// 		descriptor_buffer_info.offset = 0;
	// 		descriptor_buffer_info.range = sizeof(ModelViewProjectionMatrix);

	// 		VkDescriptorImageInfo descriptor_image_info{};
	// 		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// 		descriptor_image_info.imageView = texture_image_view_;
	// 		descriptor_image_info.sampler = texture_sampler_;

	// 		std::array<VkWriteDescriptorSet, 2> write_descriptor_sets{};
	// 		write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	// 		write_descriptor_sets[0].dstSet = descriptor_sets_[i];
	// 		write_descriptor_sets[0].dstBinding = 0;
	// 		write_descriptor_sets[0].dstArrayElement = 0;
	// 		write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// 		write_descriptor_sets[0].descriptorCount = 1;
	// 		write_descriptor_sets[0].pBufferInfo = &descriptor_buffer_info;
	// 		write_descriptor_sets[0].pImageInfo = nullptr;
	// 		write_descriptor_sets[0].pTexelBufferView = nullptr;

	// 		write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	// 		write_descriptor_sets[1].dstSet = descriptor_sets_[i];
	// 		write_descriptor_sets[1].dstBinding = 1;
	// 		write_descriptor_sets[1].dstArrayElement = 0;
	// 		write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// 		write_descriptor_sets[1].descriptorCount = 1;
	// 		write_descriptor_sets[1].pBufferInfo = nullptr;
	// 		write_descriptor_sets[1].pImageInfo = &descriptor_image_info;
	// 		write_descriptor_sets[1].pTexelBufferView = nullptr;

	// 		vkUpdateDescriptorSets(device_.GetDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
	// 	}
	// }

	// void VulkanRenderer::CreateUniformBuffers()
	// {
	// 	VkDeviceSize buffer_size = sizeof(plaincraft_render_engine::ModelViewProjectionMatrix);

	// 	auto swapchain_images = swapchain_.GetSwapchainImages();
	// 	uniform_buffers_.resize(swapchain_images.size());
	// 	uniform_buffers_memory_.resize(swapchain_images.size());

	// 	for (size_t i = 0; i < swapchain_images.size(); ++i)
	// 	{
	// 		buffer_manager_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_[i], uniform_buffers_memory_[i]);
	// 	}
	// }
}