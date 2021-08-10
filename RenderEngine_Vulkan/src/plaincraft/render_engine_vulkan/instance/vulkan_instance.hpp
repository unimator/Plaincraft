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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_INSTANCE
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_INSTANCE

#include "vulkan_instance_config.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace plaincraft_render_engine_vulkan 
{
    class VulkanInstance final
    {
        private:
            VkInstance instance_;
            VulkanInstanceConfig config_;
            VkDebugUtilsMessengerEXT debug_messanger_;

        public:
            VulkanInstance(const VulkanInstanceConfig config);
            
            VulkanInstance(const VulkanInstance& other) = delete;
            VulkanInstance(VulkanInstance&& other);

            VulkanInstance operator=(const VulkanInstance& other) = delete;
            VulkanInstance operator=(VulkanInstance&& other);

            ~VulkanInstance();

            auto GetInstance() const -> const VkInstance {return instance_;}

        private:
            void CreateInstance();
            void SetupDebugMessanger();

            bool CheckValidationLayerSupport();
            std::vector<const char*> GetRequiredExtensions();
            void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
            

            static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_INSTANCE