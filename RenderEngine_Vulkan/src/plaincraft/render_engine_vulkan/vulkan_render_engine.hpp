﻿/*
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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDER_ENGINE
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDER_ENGINE

#include "common.hpp"
#include "instance/vulkan_instance.hpp"
#include "swapchain/swapchain.hpp"
#include "device/vulkan_device.hpp"
#include "window/vulkan_window.hpp"
#include "pipeline/vulkan_pipeline.hpp"
#include "pipeline/vulkan_pipeline_config.hpp"
#include <plaincraft_render_engine.hpp>
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan {
	using namespace plaincraft_render_engine;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	class VulkanRenderEngine final : public RenderEngine {
	private:
		VulkanInstance instance_;
		VkSurfaceKHR surface_;
		VulkanDevice device_;
		Swapchain swapchain_;
		
		std::unique_ptr<VulkanPipeline> pipeline_;

		std::vector<char> vertex_shader_code_;
		std::vector<char> fragment_shader_code_;

		VkDescriptorSetLayout descriptor_set_layout_;
		VkPipelineLayout pipeline_layout_;
		
		VkCommandPool command_pool_;
		VkBuffer vertex_buffer_;
		VkDeviceMemory vertex_buffer_memory_;
		VkBuffer index_buffer_;
		VkDeviceMemory index_buffer_memory_;
		VkDescriptorPool descriptor_pool_;
		VkImage texture_image_;
		VkDeviceMemory texture_image_memory_;
		VkImageView texture_image_view_;
		VkSampler texture_sampler_;
		VkImage depth_image_;
		VkDeviceMemory depth_image_memory_;
		VkImageView depth_image_view_;

		std::vector<VkSemaphore> image_available_semaphores_;
		std::vector<VkSemaphore> render_finished_semaphores_;
		std::vector<VkFence> in_flight_fences_;
		std::vector<VkFence> images_in_flight_;
		std::vector<VkCommandBuffer> command_buffers_;
		std::vector<VkBuffer> uniform_buffers_;
		std::vector<VkDeviceMemory> uniform_buffers_memory_;
		std::vector<VkDescriptorSet> descriptor_sets_;

		bool enable_debug_ = false;
		size_t current_frame_ = 0;

	public:
		VulkanRenderEngine(std::shared_ptr<VulkanWindow> window);
		VulkanRenderEngine(std::shared_ptr<VulkanWindow>, bool enable_debug);

		VulkanRenderEngine(const VulkanRenderEngine& other) = delete;

		virtual ~VulkanRenderEngine();

		void RenderFrame() override;

	private:
		auto GetVulkanWindow() -> std::shared_ptr<VulkanWindow> { return std::static_pointer_cast<VulkanWindow>(window_); } 

		void CreateDescriptorSetLayout();
		void CreatePipelineLayout();

		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		
		uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_properties);

		void CleanupSwapChain();

		void CreateCommandPool();

		void CreateCommandBuffers();

		void CreateSyncObjects();

		void RecreateSwapChain();

		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer command_buffer);
		void CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer, VkDeviceSize size);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_image_layout, VkImageLayout new_image_layout);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void CreateDescriptorPool();
		void CreateDescriptorSets();

		void UpdateUniformBuffer(uint32_t image_index);

		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();
		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling image_tiling, VkImageUsageFlags image_usage_flags, VkMemoryPropertyFlags memory_property_flags, VkImage& image, VkDeviceMemory& image_memory);

		void CreateDepthResources();

	};
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDER_ENGINE