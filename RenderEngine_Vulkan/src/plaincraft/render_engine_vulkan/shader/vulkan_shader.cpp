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

#include "vulkan_shader.hpp"

namespace plaincraft_render_engine_vulkan {
	VulkanShader::VulkanShader(VkDevice& device, const std::vector<char>& vertex_shader, const std::vector<char>& fragment_shader) {
		VkShaderModule vertex_shader_module = CreateShaderModule(device, vertex_shader);
		VkShaderModule fragment_shader_module = CreateShaderModule(device, fragment_shader);

		VkPipelineShaderStageCreateInfo vertex_shader_stage_info{};
		vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertex_shader_stage_info.module = vertex_shader_module;
		vertex_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo fragment_shader_stage_info{};
		fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragment_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		fragment_shader_stage_info.module = fragment_shader_module;
		fragment_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_stage_info, fragment_shader_stage_info };

		vkDestroyShaderModule(device, fragment_shader_module, nullptr);
		vkDestroyShaderModule(device, vertex_shader_module, nullptr);
	}

	void VulkanShader::Use() {

	}

	void VulkanShader::SetModelViewProjection(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {

	}

	VkShaderModule VulkanShader::CreateShaderModule(VkDevice& device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module");
		}

		return shader_module;
	}
}
