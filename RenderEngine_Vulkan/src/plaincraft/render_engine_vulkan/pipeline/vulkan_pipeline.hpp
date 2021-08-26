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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_PIPELINE
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_PIPELINE

#include "../device/vulkan_device.hpp"
#include "../swapchain/swapchain.hpp"
#include "vulkan_pipeline_config.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace plaincraft_render_engine_vulkan {
    class VulkanPipeline {
    private:
        VulkanDevice& device_;

		VkPipeline graphics_pipeline_;
        VkShaderModule vertex_shader_;        
        VkShaderModule fragment_shader_;

    public:
        VulkanPipeline(VulkanDevice& device, const std::vector<char>& vertex_shader_code, const std::vector<char>& fragment_shader_code, const VulkanPipelineConfig& pipeline_config);

        ~VulkanPipeline();

        void Bind(VkCommandBuffer command_buffer);

        auto GetPipeline() -> VkPipeline {return graphics_pipeline_;}

        static void CreateDefaultPipelineConfig(VulkanPipelineConfig& pipeline_config);
    private:
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

        void CreateGraphicsPipeline(const std::vector<char>& vertex_shader_code, const std::vector<char>& fragment_shader_code, const VulkanPipelineConfig& pipeline_config);

        void CreateDescriptorSetLayout();
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_PIPELINE