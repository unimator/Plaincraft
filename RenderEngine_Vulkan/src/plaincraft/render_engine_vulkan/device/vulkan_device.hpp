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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DEVICE
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DEVICE

#include "../instance/vulkan_instance.hpp"
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan {
    class VulkanDevice final {
    private:
        VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
		VkDevice device_;

        VkQueue graphics_queue_;
        VkQueue presentation_queue_;

        VkCommandPool command_pool_;

        const std::vector<const char*> device_extensions_ = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

    public:
        VulkanDevice(const VulkanInstance& instance, VkSurfaceKHR surface);

        ~VulkanDevice();

        auto GetDevice() const -> VkDevice { return device_; }
        auto GetPhysicalDevice() const -> VkPhysicalDevice { return physical_device_; }

        auto GetGraphicsQueue() const -> VkQueue { return graphics_queue_; }
        auto GetPresentationQueue() const -> VkQueue { return presentation_queue_; }

        auto GetCommandPool() const -> VkCommandPool { return command_pool_; }

        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

    private:
        void PickPhysicalDevice(const VulkanInstance& instance, VkSurfaceKHR surface);
        void CreateLogicalDevice(VkSurfaceKHR surface);
        
        bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

        void CreateQueues(VkSurfaceKHR surface);
		void CreateCommandPool(VkSurfaceKHR surface);

        uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_properties);
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DEVICE