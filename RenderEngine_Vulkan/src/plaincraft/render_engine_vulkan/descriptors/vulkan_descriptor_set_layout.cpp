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

#include "vulkan_descriptor_set_layout.hpp"
#include <cassert>
#include <stdexcept>

namespace plaincraft_render_engine_vulkan
{
    VulkanDescriptorSetLayout::Builder::Builder(VulkanDevice &device) : device_(device) {}

    VulkanDescriptorSetLayout::Builder &VulkanDescriptorSetLayout::Builder::AddLayoutBinding(
        uint32_t binding,
        VkDescriptorType descriptor_type,
        VkShaderStageFlags shader_stage_flags,
        uint32_t descriptor_count)
    {
        assert(layout_bindings_.count(binding) == 0 && "Layout already in use");
        VkDescriptorSetLayoutBinding descriptor_set_layout_binding{};
        descriptor_set_layout_binding.binding = binding;
        descriptor_set_layout_binding.descriptorType = descriptor_type;
        descriptor_set_layout_binding.descriptorCount = descriptor_count;
        descriptor_set_layout_binding.stageFlags = shader_stage_flags;
        layout_bindings_[binding] = descriptor_set_layout_binding;
        return *this;
    }

    std::unique_ptr<VulkanDescriptorSetLayout> VulkanDescriptorSetLayout::Builder::Build() const
    {
        return std::make_unique<VulkanDescriptorSetLayout>(device_, layout_bindings_);
    }

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
        VulkanDevice &device,
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> layout_bindings)
        : device_(device), layout_bindings_(layout_bindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings{};
        for (auto layout_binding : layout_bindings)
        {
            descriptor_set_layout_bindings.push_back(layout_binding.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
        descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size());
        descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.data();

        if (vkCreateDescriptorSetLayout(
                device.GetDevice(),
                &descriptor_set_layout_create_info,
                nullptr,
                &descriptor_set_layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor set layout");
        }
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(device_.GetDevice(), descriptor_set_layout_, nullptr);
    }

    VkDescriptorSetLayout VulkanDescriptorSetLayout::GetDescriptorSetLayout() const
    {
        return descriptor_set_layout_;
    }
}