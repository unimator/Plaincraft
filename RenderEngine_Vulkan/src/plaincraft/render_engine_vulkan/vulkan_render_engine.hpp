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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDER_ENGINE
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDER_ENGINE

#include "common.hpp"
#include "queue_family_indices.hpp"
#include "swap_chain_support_details.hpp"
#include <plaincraft_render_engine.hpp>
#include <vulkan\vulkan.h>

namespace plaincraft_render_engine_vulkan {
	const int MAX_FRAMES_IN_FLIGHT = 2;

	const std::vector<const char*> VALIDATION_LAYERS = {
		"VK_LAYER_KHRONOS_validation"
	};

	class DLLEXPORT_PLAINCRAFT_RENDER_ENGINE_VULKAN VulkanRenderEngine : public plaincraft_render_engine::RenderEngine {
	private:
		VkInstance instance_;
		VkDebugUtilsMessengerEXT debug_messanger_;
		VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
		VkDevice device_;
		VkQueue graphics_queue_, presentation_queue_;
		VkSurfaceKHR surface_;
		VkSwapchainKHR swap_chain_;
		VkFormat swap_chain_image_format_;
		VkExtent2D swap_chain_extent_;
		VkRenderPass render_pass_;
		VkDescriptorSetLayout descriptor_set_layout_;
		VkPipelineLayout pipeline_layout_;
		VkPipeline graphics_pipeline_;
		VkCommandPool command_pool_;
		VkBuffer vertex_buffer_;
		VkDeviceMemory vertex_buffer_memory_;
		VkBuffer index_buffer_;
		VkDeviceMemory index_buffer_memory_;
		
		std::vector<VkSemaphore> image_available_semaphores_;
		std::vector<VkSemaphore> render_finished_semaphores_;
		std::vector<VkFence> in_flight_fences_;
		std::vector<VkFence> images_in_flight_;
		std::vector<VkImage> swap_chain_images_;
		std::vector<VkImageView> swap_chain_images_views_;
		std::vector<VkFramebuffer> swap_chain_frame_buffers_;
		std::vector<VkCommandBuffer> command_buffers_;
		std::vector<VkBuffer> uniform_buffers_;
		std::vector<VkDeviceMemory> uniform_buffers_memory_;

		bool enable_debug_ = false;
		bool frame_buffer_resized_ = false;
		size_t current_frame_ = 0;
		
		const std::vector<const char*> device_extensions_ = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

	public:
		VulkanRenderEngine(uint32_t width, uint32_t height);
		VulkanRenderEngine(uint32_t width, uint32_t height, bool enable_debug);
		virtual ~VulkanRenderEngine();

	private:
		virtual void CreateWindow();

		void CreateVulkanInstance();
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();

		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkShaderModule CreateShaderModule(VkDevice& device, const std::vector<char>& code);
		uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_properties);

		void CleanupSwapChain();

		void CreateLogicalDevice();

		void CreateSurface();

		void CreateSwapchain();

		void CreateImageViews();

		void CreateRenderPass();

		void CreateDescriptorSetLayout();

		void CreateGraphicsPipeline();

		void CreateFrameBuffers();

		void CreateCommandPool();

		void CreateCommandBuffers();

		void CreateSyncObjects();

		void RecreateSwapChain();

		void SetupDebugMessanger();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);

		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);
		void CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer, VkDeviceSize size);

		void CreateDescriptorPool();

		void UpdateUniformBuffer(uint32_t image_index);

		void RenderFrame() override;

		std::unique_ptr<plaincraft_render_engine::Shader> CreateDefaultShader() override;

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_RENDER_ENGINE