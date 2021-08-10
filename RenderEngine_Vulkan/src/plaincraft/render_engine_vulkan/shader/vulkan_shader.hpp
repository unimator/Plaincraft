/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin G�rka

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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_SHADER
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_SHADER

#include "../common.hpp"
#include <vulkan\vulkan.h>

namespace plaincraft_render_engine_vulkan {
	class VulkanShader
	{
	private:

	public:
		VulkanShader(VkDevice& device, const std::vector<char>& vertex_shader, const std::vector<char>& fragment_shader);
		virtual ~VulkanShader() = default;

		VulkanShader(const VulkanShader& other) = delete;
		VulkanShader& operator=(const VulkanShader& other) = delete;

		VulkanShader(VulkanShader&& other) noexcept {}
		VulkanShader& operator=(VulkanShader&& other) noexcept { return *this; }

	private:

		VkShaderModule CreateShaderModule(VkDevice& device, const std::vector<char>& code);
	};
}


#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_SHADER