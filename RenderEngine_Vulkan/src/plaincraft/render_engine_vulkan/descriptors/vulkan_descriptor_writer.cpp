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

#include "vulkan_descriptor_writer.hpp"
#include <cassert>

namespace plaincraft_render_engine_vulkan
{
    VulkanDescriptorWriter::VulkanDescriptorWriter(
        VulkanDescriptorSetLayout &descriptor_set_layout,
        VulkanDescriptorPool &descriptor_pool)
        : descriptor_set_layout_(descriptor_set_layout),
          descriptor_pool_(descriptor_pool)
    {
    }

    VulkanDescriptorWriter &VulkanDescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorType descriptor_type, VkDescriptorBufferInfo *buffer_info)
    {
        assert(descriptor_set_layout_.layout_bindings_.count(binding) == 1 && "Layout does not contain specified binding");

        auto &binding_description = descriptor_set_layout_.layout_bindings_[binding];

        assert(binding_description.descriptorCount == 1 &&
               "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write_descriptor_set{};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstBinding = binding;
        write_descriptor_set.pBufferInfo = buffer_info;
        write_descriptor_set.descriptorCount = 1;
        write_descriptor_set.descriptorType = descriptor_type;

        descriptor_set_writes_.push_back(write_descriptor_set);

        return *this;
    }

    VulkanDescriptorWriter &VulkanDescriptorWriter::WriteImage(uint32_t binding, VkDescriptorType descriptor_type, VkDescriptorImageInfo *image_info)
    {
        assert(descriptor_set_layout_.layout_bindings_.count(binding) == 1 && "Layout does not contain specified binding");

        auto &binding_description = descriptor_set_layout_.layout_bindings_[binding];

        assert(binding_description.descriptorCount == 1 &&
               "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write_descriptor_set{};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstBinding = binding;
        write_descriptor_set.pImageInfo = image_info;
        write_descriptor_set.descriptorCount = 1;
        write_descriptor_set.descriptorType = descriptor_type;

        descriptor_set_writes_.push_back(write_descriptor_set);

        return *this;
    }

    bool VulkanDescriptorWriter::Build(VkDescriptorSet &descriptor_set)
    {
        bool success = descriptor_pool_.AllocateDescriptor(descriptor_set_layout_.GetDescriptorSetLayout(), descriptor_set);

        if (!success)
        {
            return false;
        }
        Overwrite(descriptor_set);
        return true;
    }

    void VulkanDescriptorWriter::Overwrite(VkDescriptorSet &descriptor_set)
    {
        for (auto &descriptor_set_write : descriptor_set_writes_)
        {
            descriptor_set_write.dstSet = descriptor_set;
        }

        vkUpdateDescriptorSets(descriptor_pool_.device_.GetDevice(), descriptor_set_writes_.size(), descriptor_set_writes_.data(), 0, nullptr);
    }
}