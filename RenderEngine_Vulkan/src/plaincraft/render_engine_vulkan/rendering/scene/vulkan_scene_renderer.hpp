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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_SCENE_RENDERER
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_SCENE_RENDERER

#include "../../device/vulkan_device.hpp"
#include "../../swapchain/vulkan_swapchain.hpp"
#include "../../pipeline/vulkan_pipeline.hpp"
#include "../../memory/vulkan_buffer.hpp"
#include "../../models/vulkan_model.hpp"
#include "../vulkan_renderer_frame_config.hpp"
#include "../../descriptors/vulkan_descriptor_set_layout.hpp"
#include "../../descriptors/vulkan_descriptor_pool.hpp"
#include "../../descriptors/vulkan_descriptor_writer.hpp"
#include <plaincraft_render_engine.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan {
    using namespace plaincraft_render_engine;

    struct SimplePushConstants 
    {
        glm::mat4 transform;
    };

    class VulkanSceneRenderer : public SceneRenderer {
    private:
        VulkanDevice& device_;
        VkRenderPass render_pass_;
        VkExtent2D extent_;
        size_t images_count_;
        
		std::vector<char> vertex_shader_code_;
		std::vector<char> fragment_shader_code_;

        std::unique_ptr<VulkanPipeline> pipeline_;
		VkPipelineLayout pipeline_layout_;
        
		std::unique_ptr<VulkanDescriptorPool> descriptor_pool_;
		std::unique_ptr<VulkanDescriptorSetLayout> descriptor_set_layout_;
		std::vector<VkDescriptorSet> descriptor_sets_;

        std::vector<std::unique_ptr<VulkanBuffer>> mvp_uniform_buffers_;
        
        std::vector<std::unique_ptr<VulkanBuffer>> model_buffers_;
        std::vector<std::unique_ptr<VulkanBuffer>> view_projection_buffers_;
		        
        // TODO: get rid of these
		VkImageView texture_image_view_;
		VkSampler texture_sampler_;

    public:
        VulkanSceneRenderer(VulkanDevice& device, VkRenderPass render_pass, VkExtent2D extent, size_t images_count, std::shared_ptr<Camera> camera);
        ~VulkanSceneRenderer() override;

        void Render(VulkanRendererFrameConfig& frame_config);
        auto GetLayout() const -> VkPipelineLayout { return pipeline_layout_; }
        
        //void UpdateUniformBuffer(uint32_t image_index);

    private:
		void SetupPipelineConfig(VulkanPipelineConfig& pipeline_config, VkViewport& viewport, VkRect2D& scissor);
		void CreatePipelineLayout();
        
		void CreateUniformBuffers();

        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateDescriptors();

        void RecreateEntitiesBuffers();
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_SCENE_RENDERER