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

#include "vulkan_descriptor_pool.hpp"
#include <stdexcept>

namespace plaincraft_render_engine_vulkan
{
    VulkanDescriptorPool::Builder::Builder(VulkanDevice &device) : device_(device) {}

    VulkanDescriptorPool::Builder &VulkanDescriptorPool::Builder::AddPoolSize(
        VkDescriptorType descriptor_type,
        uint32_t count)
    {
        descriptor_pool_sizes_.push_back({descriptor_type, count});
        return *this;
    }

    VulkanDescriptorPool::Builder &VulkanDescriptorPool::Builder::SetPoolFlags(
        VkDescriptorPoolCreateFlags descriptor_pool_flags)
    {
        descriptor_pool_flags_ = descriptor_pool_flags;
        return *this;
    }

    VulkanDescriptorPool::Builder &VulkanDescriptorPool::Builder::SetMaxSets(uint32_t count)
    {
        max_sets_ = count;
        return *this;
    }

    std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPool::Builder::Build() const
    {
        return std::make_unique<VulkanDescriptorPool>(device_, max_sets_, descriptor_pool_flags_, descriptor_pool_sizes_);
    }

    VulkanDescriptorPool::VulkanDescriptorPool(
        VulkanDevice &device,
        uint32_t max_sets,
        VkDescriptorPoolCreateFlags descriptor_pool_flags,
        const std::vector<VkDescriptorPoolSize> &pool_sizes)
        : device_(device)
    {
        VkDescriptorPoolCreateInfo descriptor_pool_create_info{};
        descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
        descriptor_pool_create_info.maxSets = max_sets;
        descriptor_pool_create_info.flags = descriptor_pool_flags;

        if (vkCreateDescriptorPool(device.GetDevice(), &descriptor_pool_create_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool");
        }
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        vkDestroyDescriptorPool(device_.GetDevice(), descriptor_pool_, nullptr);
    }

    bool VulkanDescriptorPool::AllocateDescriptor(
        const VkDescriptorSetLayout descriptor_set_layout,
        VkDescriptorSet &descriptor_set) const
    {
        VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
        descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptor_set_allocate_info.descriptorPool = descriptor_pool_;
        descriptor_set_allocate_info.pSetLayouts = &descriptor_set_layout;
        descriptor_set_allocate_info.descriptorSetCount = 1;

        auto result = vkAllocateDescriptorSets(device_.GetDevice(), &descriptor_set_allocate_info, &descriptor_set);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    void VulkanDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet> &descriptor_sets) const
    {
        vkFreeDescriptorSets(
            device_.GetDevice(),
            descriptor_pool_,
            static_cast<uint32_t>(descriptor_sets.size()),
            descriptor_sets.data());
    }

    void VulkanDescriptorPool::ResetPool()
    {
        vkResetDescriptorPool(device_.GetDevice(), descriptor_pool_, 0);
    }
}