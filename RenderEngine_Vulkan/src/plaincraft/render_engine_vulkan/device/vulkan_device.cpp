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

#include "vulkan_device.hpp"
#include "../utils/queue_family.hpp"
#include "../utils/validation_layers.hpp"
#include "../swapchain/swapchain.hpp"
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <set>
#include <string>

namespace plaincraft_render_engine_vulkan {
    VulkanDevice::VulkanDevice(const VulkanInstance& instance, VkSurfaceKHR surface) 
    {
        PickPhysicalDevice(instance, surface);
        CreateLogicalDevice(surface);
    }

    VulkanDevice::~VulkanDevice()
    {
		vkDestroyDevice(device_, nullptr);
    }

	void VulkanDevice::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
    {
        VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vertex buffer");
		}

		VkMemoryRequirements memory_requirements;
		vkGetBufferMemoryRequirements(device_, buffer, &memory_requirements);

		VkMemoryAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.allocationSize = memory_requirements.size;
		allocate_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, memory_properties);

		if (vkAllocateMemory(device_, &allocate_info, nullptr, &buffer_memory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate vertex buffer memory");
		}

		vkBindBufferMemory(device_, buffer, buffer_memory, 0);
    }

    void VulkanDevice::PickPhysicalDevice(const VulkanInstance& instance, VkSurfaceKHR surface)
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(instance.GetInstance(), &device_count, nullptr);

		if (device_count == 0)
		{
			throw std::runtime_error("Failed to find GPUs with Vulkan support");
		}

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(instance.GetInstance(), &device_count, devices.data());

		for (const auto &device : devices)
		{
			if (IsDeviceSuitable(device, surface))
			{
				physical_device_ = device;
				break;
			}
		}

		if (physical_device_ == VK_NULL_HANDLE)
		{
			throw std::runtime_error("Failed to find suitable device");
		}
	}

    void VulkanDevice::CreateLogicalDevice(VkSurfaceKHR surface)
    {
        auto indices = FindQueueFamilyIndices(physical_device_, surface);

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

		float queue_priority = 1.0f;
		for (auto queue_family : unique_queue_families)
		{
			VkDeviceQueueCreateInfo device_queue_create_info{};
			device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_info.queueFamilyIndex = queue_family;
			device_queue_create_info.queueCount = 1;
			device_queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(device_queue_create_info);
		}

		VkPhysicalDeviceFeatures device_feauters{};
		device_feauters.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(unique_queue_families.size());
		device_create_info.pEnabledFeatures = &device_feauters;
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions_.size());
		device_create_info.ppEnabledExtensionNames = device_extensions_.data();

#ifndef NDEBUG
        device_create_info.enabledLayerCount = 0;
#else
        device_create_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        device_create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif // NDEBUG

		if (vkCreateDevice(physical_device_, &device_create_info, nullptr, &device_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create logical device");
		}
    }

    bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		auto indices = FindQueueFamilyIndices(device, surface);

		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(device, &device_properties);

		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceFeatures(device, &device_features);

		const auto extensions_supported = CheckDeviceExtensionSupport(device);
		auto swap_chain_adequate = false;
		if (extensions_supported)
		{
			auto swap_chain_support_details = QuerySwapChainSupport(device, surface);
			swap_chain_adequate = !swap_chain_support_details.formats.empty() && !swap_chain_support_details.present_modes.empty();
		}

		return indices.IsComplete() && device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader && extensions_supported && swap_chain_adequate && device_features.samplerAnisotropy;
	}

	bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensions_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, nullptr);

		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, available_extensions.data());

		std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());

		for (const auto &extension : available_extensions)
		{
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	uint32_t VulkanDevice::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_properties)
	{
		VkPhysicalDeviceMemoryProperties device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device_, &device_memory_properties);

		for (uint32_t i = 0; i < device_memory_properties.memoryTypeCount; ++i) {
			if ((type_filter & (1 << i)) && (device_memory_properties.memoryTypes[i].propertyFlags & memory_properties) == memory_properties) {
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}
}