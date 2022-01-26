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

#include "vulkan_scene_renderer.hpp"
#include <chrono>
#include <fstream>
#include <glm\gtx\quaternion.hpp>

namespace plaincraft_render_engine_vulkan
{
	VulkanSceneRenderer::VulkanSceneRenderer(VulkanDevice &device, VkRenderPass render_pass, VkExtent2D extent, size_t images_count, std::shared_ptr<Camera> camera)
		: SceneRenderer(camera),
		  device_(device),
		  render_pass_(render_pass),
		  extent_(extent),
		  images_count_(images_count)
	{
		vertex_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\vert.spv");
		fragment_shader_code_ = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\frag.spv");

		VulkanPipelineConfig pipeline_config{};
		CreateDescriptorSetLayout();
		CreateDescriptorPool();
		CreateUniformBuffers();
		CreateDescriptors();
		CreatePipelineLayout();
		VkViewport viewport{};
		VkRect2D scissor{};
		SetupPipelineConfig(pipeline_config, viewport, scissor);

		pipeline_ = std::make_unique<VulkanPipeline>(device_, vertex_shader_code_, fragment_shader_code_, pipeline_config);
	}

	VulkanSceneRenderer::~VulkanSceneRenderer()
	{
		vkDestroyPipelineLayout(device_.GetDevice(), pipeline_layout_, nullptr);
	}

	void VulkanSceneRenderer::Render(VulkanRendererFrameConfig &frame_config)
	{
		auto command_buffer = frame_config.command_buffer;
		auto descriptor_set = descriptor_sets_[frame_config.image_index];
		pipeline_->Bind(command_buffer);
		glm::mat4 projection = glm::perspective(glm::radians(camera_->fov), (float)1024 / (float)768, 0.1f, 100.0f);
		projection[1][1] *= -1;
		glm::mat4 view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);
		auto& mvp_uniform_buffer = mvp_uniform_buffers_[frame_config.image_index];
		auto alignment_size = mvp_uniform_buffer->GetAlignmentSize();

		for (auto i = 0; i < drawables_list_.size(); ++i)
		{
			auto drawable = drawables_list_[i];
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
			mvp_uniform_buffer->Map(alignment_size, offset);
			mvp_uniform_buffer->Write(&mvp, alignment_size, 0);
			mvp_uniform_buffer->Unmap();

			uint32_t dynamic_offset = i * alignment_size;
			vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1, &descriptor_set, 1, &dynamic_offset);
			auto draw_model = drawable->GetModel();
			if (draw_model.expired())
			{
				continue;
			}
			auto vulkan_model = std::dynamic_pointer_cast<VulkanModel>(draw_model.lock());
			vulkan_model->Bind(command_buffer);
			vulkan_model->Draw(command_buffer);
			vulkan_model.reset();
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
		}
	}

	void VulkanSceneRenderer::CreatePipelineLayout()
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(SimplePushConstants);

		auto descriptor_set_layout_value = descriptor_set_layout_->GetDescriptorSetLayout();
		VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout_value;
		pipeline_layout_create_info.pushConstantRangeCount = 1;
		pipeline_layout_create_info.pPushConstantRanges = &push_constant_range;
		pipeline_layout_create_info.pNext = VK_NULL_HANDLE;

		if (vkCreatePipelineLayout(device_.GetDevice(), &pipeline_layout_create_info, nullptr, &pipeline_layout_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void VulkanSceneRenderer::SetupPipelineConfig(VulkanPipelineConfig &pipeline_config, VkViewport &viewport, VkRect2D &scissor)
	{
		pipeline_config.descriptor_set_layout = descriptor_set_layout_->GetDescriptorSetLayout();
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

	// void VulkanSceneRenderer::CreateDescriptorSetLayout()
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

	void VulkanSceneRenderer::RecreateEntitiesBuffers()
	{
		auto entities_count = drawables_list_.size();
	}

	void VulkanSceneRenderer::CreateUniformBuffers()
	{
		VkPhysicalDeviceProperties physical_device_properties;
		vkGetPhysicalDeviceProperties(device_.GetPhysicalDevice(), &physical_device_properties);
		auto min_ubo_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;

		mvp_uniform_buffers_.resize(images_count_);

		uint32_t instance_count = (10 * 40 * 40 + 1);

		for (size_t i = 0; i < images_count_; ++i)
		{
			mvp_uniform_buffers_[i] = std::make_unique<VulkanBuffer>(device_, sizeof(ModelViewProjectionMatrix), instance_count, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, min_ubo_alignment);
		}
	}

	void VulkanSceneRenderer::CreateDescriptorSetLayout()
	{
		descriptor_set_layout_ = VulkanDescriptorSetLayout::Builder(device_)
			.AddLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 1)
			//.AddLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.Build();
	}

	void VulkanSceneRenderer::CreateDescriptorPool()
	{
		descriptor_pool_ = VulkanDescriptorPool::Builder(device_)
							   .SetMaxSets(images_count_)
							   .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1)
							   //.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
							   .Build();
	}

	void VulkanSceneRenderer::CreateDescriptors()
	{
		descriptor_sets_.resize(images_count_);
		for (size_t i = 0; i < descriptor_sets_.size(); ++i)
		{
			VkDescriptorBufferInfo descriptor_buffer_info{};
			descriptor_buffer_info.buffer = mvp_uniform_buffers_[i]->GetBuffer();
			descriptor_buffer_info.offset = 0;
			descriptor_buffer_info.range = sizeof(ModelViewProjectionMatrix);

			// VkDescriptorImageInfo descriptor_image_info{};
			// descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			// descriptor_image_info.imageView = texture_image_view_->GetImageView();
			// descriptor_image_info.sampler = texture_image_->GetSampler();

			VulkanDescriptorWriter descriptor_writer(*descriptor_set_layout_, *descriptor_pool_);
			descriptor_writer.WriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, &descriptor_buffer_info);
			//descriptor_writer.WriteImage(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &descriptor_image_info);
			descriptor_writer.Build(descriptor_sets_[i]);
		}
	}
}