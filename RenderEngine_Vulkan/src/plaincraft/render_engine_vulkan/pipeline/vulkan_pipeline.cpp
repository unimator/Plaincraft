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

#include "vulkan_pipeline.hpp"
#include "../renderer/vertex_utils.hpp"
#include "../device/vulkan_device.hpp"
#include <stdexcept>

namespace plaincraft_render_engine_vulkan
{
	VulkanPipeline::VulkanPipeline(const VulkanDevice &device, const std::vector<char> &vertex_shader_code, const std::vector<char> &fragment_shader_code, const VulkanPipelineConfig &pipeline_config)
		: device_(device)
	{
		CreateGraphicsPipeline(vertex_shader_code, fragment_shader_code, pipeline_config);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyPipeline(device_.GetDevice(), graphics_pipeline_, nullptr);
	}

	void VulkanPipeline::Bind(VkCommandBuffer command_buffer)
	{
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
	}

	void VulkanPipeline::CreateGraphicsPipeline(const std::vector<char> &vertex_shader_code, const std::vector<char> &fragment_shader_code, const VulkanPipelineConfig &pipeline_config)
	{
		auto vk_device = device_.GetDevice();

		auto vertex_shader_module = CreateShaderModule(vertex_shader_code);
		auto fragment_shader_module = CreateShaderModule(fragment_shader_code);

		VkPipelineShaderStageCreateInfo vertex_shader_stage_info{};
		vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertex_shader_stage_info.module = vertex_shader_module;
		vertex_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo fragment_shader_stage_info{};
		fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragment_shader_stage_info.module = fragment_shader_module;
		fragment_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_info, fragment_shader_stage_info};

		auto binding_description = VertexUtils::GetBindingDescription();
		auto attribute_description = VertexUtils::GetAttributeDescription();

		VkPipelineVertexInputStateCreateInfo vertex_input_info{};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.pVertexBindingDescriptions = &binding_description;
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.size());
		vertex_input_info.pVertexAttributeDescriptions = attribute_description.data();
		
		VkDynamicState dynamic_states[] = {
			VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH
		};

		/*VkPipelineDynamicStateCreateInfo dynamic_state_info{};
		dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state_info.dynamicStateCount = 2;
		dynamic_state_info.pDynamicStates = dynamic_states;*/

		VkGraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &pipeline_config.input_assembly_info;
		pipeline_info.pRasterizationState = &pipeline_config.rasterization_info;
		pipeline_info.pMultisampleState = &pipeline_config.multisample_info;
		pipeline_info.pDepthStencilState = &pipeline_config.depth_stencil_info;
		pipeline_info.pColorBlendState = &pipeline_config.color_blend_info;
		//pipeline_info.pDynamicState = &dynamic_state_info;
		pipeline_info.pViewportState = &pipeline_config.viewport_info;
		pipeline_info.layout = pipeline_config.pipeline_layout;
		pipeline_info.renderPass = pipeline_config.render_pass;
		pipeline_info.subpass = pipeline_config.subpass;
		//pipeline_info.pDynamicState = &pipeline_config.dynamic_state_info;
		pipeline_info.pDynamicState = nullptr;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
		pipeline_info.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(vk_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}

		vkDestroyShaderModule(vk_device, fragment_shader_module, nullptr);
		vkDestroyShaderModule(vk_device, vertex_shader_module, nullptr);
	}

	VkShaderModule VulkanPipeline::CreateShaderModule(const std::vector<char> &code)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(device_.GetDevice(), &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}

		return shader_module;
	}

	void VulkanPipeline::CreateDefaultPipelineConfig(VulkanPipelineConfig &pipeline_config)
	{
		pipeline_config.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipeline_config.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline_config.input_assembly_info.primitiveRestartEnable = VK_FALSE;

		pipeline_config.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipeline_config.viewport_info.viewportCount = 1;
		pipeline_config.viewport_info.pViewports = nullptr;
		pipeline_config.viewport_info.scissorCount = 1;
		pipeline_config.viewport_info.pScissors = nullptr;

		pipeline_config.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipeline_config.rasterization_info.depthClampEnable = VK_FALSE;
		pipeline_config.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		pipeline_config.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
		pipeline_config.rasterization_info.lineWidth = 1.0f;
		pipeline_config.rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
		pipeline_config.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		pipeline_config.rasterization_info.depthBiasEnable = VK_FALSE;
		pipeline_config.rasterization_info.depthBiasConstantFactor = 0.0f;
		pipeline_config.rasterization_info.depthBiasClamp = 0.0f;
		pipeline_config.rasterization_info.depthBiasSlopeFactor = 0.0f;

		pipeline_config.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipeline_config.multisample_info.sampleShadingEnable = VK_FALSE;
		pipeline_config.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipeline_config.multisample_info.minSampleShading = 1.0f;
		pipeline_config.multisample_info.pSampleMask = nullptr;
		pipeline_config.multisample_info.alphaToCoverageEnable = VK_FALSE;
		pipeline_config.multisample_info.alphaToOneEnable = VK_FALSE;

		pipeline_config.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		pipeline_config.color_blend_attachment.blendEnable = VK_FALSE;
		pipeline_config.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		pipeline_config.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipeline_config.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		pipeline_config.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		pipeline_config.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipeline_config.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

		pipeline_config.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipeline_config.color_blend_info.logicOpEnable = VK_FALSE;
		pipeline_config.color_blend_info.logicOp = VK_LOGIC_OP_COPY;
		pipeline_config.color_blend_info.attachmentCount = 1;
		pipeline_config.color_blend_info.pAttachments = &pipeline_config.color_blend_attachment;
		pipeline_config.color_blend_info.blendConstants[0] = 0.0f;
		pipeline_config.color_blend_info.blendConstants[1] = 0.0f;
		pipeline_config.color_blend_info.blendConstants[2] = 0.0f;
		pipeline_config.color_blend_info.blendConstants[3] = 0.0f;

		pipeline_config.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipeline_config.depth_stencil_info.depthTestEnable = VK_TRUE;
		pipeline_config.depth_stencil_info.depthWriteEnable = VK_TRUE;
		pipeline_config.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
		pipeline_config.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
		pipeline_config.depth_stencil_info.minDepthBounds = 0.0f;
		pipeline_config.depth_stencil_info.maxDepthBounds = 1.0f;
		pipeline_config.depth_stencil_info.stencilTestEnable = VK_FALSE;
		pipeline_config.depth_stencil_info.front = {};
		pipeline_config.depth_stencil_info.back = {};

		/*pipeline_config.dynamic_state_enables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		pipeline_config.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipeline_config.dynamic_state_info.pDynamicStates = pipeline_config.dynamic_state_enables.data();
		pipeline_config.dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(pipeline_config.dynamic_state_enables.size());
		pipeline_config.dynamic_state_info.flags = 0;*/
	}
}
