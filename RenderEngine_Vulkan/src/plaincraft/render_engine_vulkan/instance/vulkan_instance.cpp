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

#include "vulkan_instance.hpp"
#include "../utils/validation_layers.hpp"
#include "../common.hpp"
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace plaincraft_render_engine_vulkan
{
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return static_cast<VkResult>(func(instance, pCreateInfo, pAllocator, pDebugMessenger));
		}
		else {
			return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

    VulkanInstance::VulkanInstance(const VulkanInstanceConfig config)
        : config_(config)
    {
        CreateInstance();
        SetupDebugMessanger();
    }

	VulkanInstance::VulkanInstance(VulkanInstance&& other)
	{
		this->config_ = other.config_;
		
		this->instance_ = other.instance_;
		other.instance_ = VK_NULL_HANDLE;

		this->debug_messanger_ = other.debug_messanger_;
		other.debug_messanger_ = VK_NULL_HANDLE;
	}

    VulkanInstance::~VulkanInstance()
    {
        if (config_.enable_debug) {
			DestroyDebugUtilsMessengerEXT(instance_, debug_messanger_, nullptr);
		}
		vkDestroyInstance(instance_, nullptr);
    }

    void VulkanInstance::CreateInstance()
    {
        if (config_.enable_debug && !CheckValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested, but not available!");
		}

        VkApplicationInfo application_info{};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pNext = nullptr;
		application_info.pApplicationName = config_.application_name.c_str();
		application_info.applicationVersion = config_.application_version;
		application_info.pEngineName = config_.engine_name.c_str();
		application_info.engineVersion = config_.engine_version;
		application_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &application_info;

		auto extensions = GetRequiredExtensions();
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debug_create_info;

		if (config_.enable_debug) {
			create_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
			create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();

			PopulateDebugMessengerCreateInfo(debug_create_info);
			create_info.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_create_info);
		}
		else {
			create_info.enabledLayerCount = 0;
			create_info.pNext = nullptr;
		}

		if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan instance");
		}
    }

    void VulkanInstance::SetupDebugMessanger()
    {
        if (!config_.enable_debug) {
			return;
		}

		VkDebugUtilsMessengerCreateInfoEXT create_info;
		PopulateDebugMessengerCreateInfo(create_info);

		if (CreateDebugUtilsMessengerEXT(instance_, &create_info, nullptr, &debug_messanger_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to set up debug messenger!");
		}
    }

    bool VulkanInstance::CheckValidationLayerSupport() 
    {
        uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		for (const char* layer_name : VALIDATION_LAYERS) {
			bool layer_found = false;

			for (const auto& layer_properties : available_layers) {
				if (strcmp(layer_name, layer_properties.layerName) == 0) {
					layer_found = true;
					break;
				}
			}

			if (!layer_found) {
				return false;
			}
		}

		return true;
    }

    std::vector<const char*> VulkanInstance::GetRequiredExtensions() {
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		if (config_.enable_debug) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		uint32_t extensions_count;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());
		
		for (const char* extension : extensions) {
			bool extension_found = false;

			for (const auto& extension_property : available_extensions) {
				if (strcmp(extension, extension_property.extensionName) == 0) {
					extension_found = true;
					break;
				}
			}

			if (!extension_found) {
				throw std::runtime_error("Not all extensions are available");
			}
 		}

		return extensions;
	}

    void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) 
    {
        create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = DebugCallback;
		create_info.pUserData = nullptr;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
		std::cerr << "Validation layer: " << callback_data->pMessage << std::endl;

		return VK_FALSE;
	}
}