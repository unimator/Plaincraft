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
		RecreateEntitiesBuffers();
		CreatePipelineLayout();
		VkViewport viewport{};
		VkRect2D scissor{};
		SetupPipelineConfig(pipeline_config, viewport, scissor);

		pipeline_ = std::make_unique<VulkanPipeline>(device_, vertex_shader_code_, fragment_shader_code_, pipeline_config);
	}

	VulkanSceneRenderer::~VulkanSceneRenderer()
	{
		vkDestroyPipelineLayout(device_.GetDevice(), pipeline_layout_, nullptr);

		if(model_memory_ != nullptr)
		{
			_aligned_free(model_memory_);
		}
	}

	void VulkanSceneRenderer::Render(VulkanRendererFrameConfig &frame_config)
	{
		if (drawables_list_.size() > buffers_instance_count_)
		{
			RecreateEntitiesBuffers();
		}

		auto command_buffer = frame_config.command_buffer;
		pipeline_->Bind(command_buffer);

		auto descriptor_set = descriptor_sets_[frame_config.image_index];

		glm::mat4 projection = glm::perspective(glm::radians(camera_->fov), (float)extent_.width / (float)extent_.height, 0.1f, 100.0f);
		projection[1][1] *= -1;
		glm::mat4 view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);

		auto &view_projection_buffer = view_projection_buffers_[frame_config.image_index];
		auto &model_buffer = model_buffers_[frame_config.image_index];
		auto alignment_size = view_projection_buffer->GetAlignmentSize();

		ViewProjectionMatrix vp_matrix {
			view,
			projection
		};

		view_projection_buffer->Map(sizeof(ViewProjectionMatrix), 0);
		view_projection_buffer->Write(&vp_matrix, sizeof(ViewProjectionMatrix), 0);
		view_projection_buffer->Unmap();
		//view_projection_buffer->Flush(sizeof(ViewProjectionMatrix), 0);

		for (auto i = 0; i < drawables_list_.size(); ++i)
		{
			auto drawable = drawables_list_[i];
			auto color = drawable->GetColor();
			auto model = drawable->GetModelMatrix();

			auto offset = static_cast<uint32_t>(i) * alignment_size;
			auto *model_matrix = reinterpret_cast<ModelMatrix*>((reinterpret_cast<uint64_t>(model_memory_) + (i * alignment_size)));
			model_matrix->color = color;
			model_matrix->model = model;
		}
		
		model_buffer->Map(alignment_size * drawables_list_.size(), 0);
		model_buffer->Write(model_memory_, alignment_size * drawables_list_.size(), 0);
		model_buffer->Unmap();
		model_buffer->Flush(alignment_size * drawables_list_.size(), 0);
		
		std::shared_ptr<VulkanModel> previous_model = nullptr;
		for (auto i = 0; i < drawables_list_.size(); ++i)
		{
			auto drawable = drawables_list_[i];
			uint32_t dynamic_offset = i * alignment_size;
			vkCmdBindDescriptorSets(command_buffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline_layout_,
				0,
				1,
				&descriptor_set,
				1,
				&dynamic_offset);

			auto draw_model = drawable->GetModel();
			if (draw_model.expired())
			{
				continue;
			}

			auto vulkan_model = std::dynamic_pointer_cast<VulkanModel>(draw_model.lock());
			vulkan_model->Bind(command_buffer);
			vulkan_model->Draw(command_buffer);
			vulkan_model.reset();
		}
	}

	VkPipelineLayout VulkanSceneRenderer::GetLayout() const
	{
		return pipeline_layout_;
	}

	void VulkanSceneRenderer::Check()
	{
		if (drawables_list_.size() > buffers_instance_count_)
		{
			RecreateEntitiesBuffers();
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

	void VulkanSceneRenderer::RecreateEntitiesBuffers()
	{
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptors();
	}

	void VulkanSceneRenderer::CreateUniformBuffers()
	{
		VkPhysicalDeviceProperties physical_device_properties;
		vkGetPhysicalDeviceProperties(device_.GetPhysicalDevice(), &physical_device_properties);
		auto min_ubo_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;

		view_projection_buffers_.resize(images_count_);
		model_buffers_.resize(images_count_);

		auto next_power_of_2 = [](uint32_t number) -> uint32_t
		{
			uint32_t count = 0;

			if (number && !(number & (number - 1)))
			{
				return number;
			}

			while (number != 0)
			{
				number >>= 1;
				count += 1;
			}

			return 1 << count;
		};

		auto entities_count = static_cast<uint32_t>(drawables_list_.size());
		if (entities_count == 0)
		{
			entities_count = 1;
		}

		buffers_instance_count_ = next_power_of_2(entities_count);

		for (size_t i = 0; i < images_count_; ++i)
		{
			view_projection_buffers_[i] = std::make_unique<VulkanBuffer>(device_,
																		 sizeof(ViewProjectionMatrix),
																		 1,
																		 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
																		 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
																		 min_ubo_alignment);

			model_buffers_[i] = std::make_unique<VulkanBuffer>(device_,
															   sizeof(ModelMatrix),
															   buffers_instance_count_,
															   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
															   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
															   min_ubo_alignment);
		}

		if(model_memory_ != nullptr)
		{
			_aligned_free(model_memory_);
		}
		model_memory_ = static_cast<ModelMatrix*>(_aligned_malloc(model_buffers_[0]->GetAlignmentSize() * buffers_instance_count_, model_buffers_[0]->GetAlignmentSize()));
	}

	void VulkanSceneRenderer::CreateDescriptorSetLayout()
	{
		descriptor_set_layout_ = VulkanDescriptorSetLayout::Builder(device_)
									 .AddLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 1)
									 .AddLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
									 //.AddLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
									 .Build();
	}

	void VulkanSceneRenderer::CreateDescriptorPool()
	{
		descriptor_pool_ = VulkanDescriptorPool::Builder(device_)
							   .SetMaxSets(images_count_)
							   .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1)
							   .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
							   //.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
							   .Build();
	}

	void VulkanSceneRenderer::CreateDescriptors()
	{
		descriptor_sets_.resize(images_count_);
		for (size_t i = 0; i < descriptor_sets_.size(); ++i)
		{
			VkDescriptorBufferInfo model_buffer_descriptor_info{};
			model_buffer_descriptor_info.buffer = model_buffers_[i]->GetBuffer();
			model_buffer_descriptor_info.offset = 0;
			model_buffer_descriptor_info.range = sizeof(ModelMatrix);

			VkDescriptorBufferInfo view_projection_buffer_descriptor_info{};
			view_projection_buffer_descriptor_info.buffer = view_projection_buffers_[i]->GetBuffer();
			view_projection_buffer_descriptor_info.offset = 0;
			view_projection_buffer_descriptor_info.range = sizeof(ViewProjectionMatrix);

			// VkDescriptorImageInfo descriptor_image_info{};
			// descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			// descriptor_image_info.imageView = texture_image_view_->GetImageView();
			// descriptor_image_info.sampler = texture_image_->GetSampler();

			VulkanDescriptorWriter descriptor_writer(*descriptor_set_layout_, *descriptor_pool_);
			descriptor_writer.WriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, &model_buffer_descriptor_info);
			descriptor_writer.WriteBuffer(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &view_projection_buffer_descriptor_info);
			// descriptor_writer.WriteImage(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &descriptor_image_info);
			descriptor_writer.Build(descriptor_sets_[i]);
		}
	}
}