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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_SET_LAYOUT
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_SET_LAYOUT

#include "../device/vulkan_device.hpp"
#include <unordered_map>
#include <memory>

namespace plaincraft_render_engine_vulkan
{
    class VulkanDescriptorWriter;

    class VulkanDescriptorSetLayout
    {
    public:
        class Builder final
        {
        private:
            VulkanDevice &device_;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> layout_bindings_;

        public:
            Builder(VulkanDevice &device);

            Builder &AddLayoutBinding(
                uint32_t binding,
                VkDescriptorType descriptor_type,
                VkShaderStageFlags shader_stage_flags,
                uint32_t descriptor_count = 1);

            std::unique_ptr<VulkanDescriptorSetLayout> Build() const;
        };

    private:
        VulkanDevice &device_;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> layout_bindings_;
        VkDescriptorSetLayout descriptor_set_layout_;

        friend class VulkanDescriptorWriter;

    public:
        VulkanDescriptorSetLayout(VulkanDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> layout_bindings);
        ~VulkanDescriptorSetLayout();

        VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout &other) = delete;
        VulkanDescriptorSetLayout &operator=(const VulkanDescriptorSetLayout &other) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const;
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_SET_LAYOUT