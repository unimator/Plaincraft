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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDERER
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDERER

#include "../device/vulkan_device.hpp"
#include "../swapchain/swapchain.hpp"
#include "../pipeline/vulkan_pipeline.hpp"
#include "../memory/vulkan_buffer_manager.hpp"
#include "../model/vulkan_model.hpp"
#include <plaincraft_render_engine.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan {
    using namespace plaincraft_render_engine;

    class VulkanRenderer : public Renderer {
    private:
        const VulkanDevice& device_;
        Swapchain& swapchain_;

        std::unique_ptr<VulkanPipeline> pipeline_;
		VkPipelineLayout pipeline_layout_;
        
		VkDescriptorPool descriptor_pool_;
		VkDescriptorSetLayout descriptor_set_layout_;
        std::vector<VkDescriptorSet> descriptor_sets_;

		std::vector<char> vertex_shader_code_;
		std::vector<char> fragment_shader_code_;
        
		std::vector<VkCommandBuffer> command_buffers_;
		std::vector<VkBuffer> uniform_buffers_;
		std::vector<VkDeviceMemory> uniform_buffers_memory_;

        VulkanBufferManager buffer_manager_;
        
        // TODO: get rid of these
		VkImageView texture_image_view_;
		VkSampler texture_sampler_;
        
		std::unique_ptr<VulkanModel> model_;
        
		void LoadModel();

    public:
        VulkanRenderer(const VulkanDevice& device, Swapchain& swapchain_, std::shared_ptr<Camera> camera, VkImageView texture_view, VkSampler texture_sampler);
        ~VulkanRenderer() override;

        void Render() override;
        
        void CreateCommandBuffers();
        void UpdateUniformBuffer(uint32_t image_index);
        
        auto GetCommandBuffer(uint32_t index) -> VkCommandBuffer { return command_buffers_[index]; }

    private:
		void SetupPipelineConfig(VulkanPipelineConfig& pipeline_config, VkViewport& viewport, VkRect2D& scissor);
		void CreatePipelineLayout();

        void CreateDescriptorPool();
        void CreateDescriptorSetLayout();
        void CreateDescriptorSets();

        void CreateUniformBuffers();
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDERER