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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_WRITER
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_WRITER

#include "vulkan_descriptor_pool.hpp"
#include "vulkan_descriptor_set_layout.hpp"
#include <vector>

namespace plaincraft_render_engine_vulkan {
    class VulkanDescriptorWriter {
    private:
        VulkanDescriptorSetLayout& descriptor_set_layout_;
        VulkanDescriptorPool& descriptor_pool_;
        std::vector<VkWriteDescriptorSet> descriptor_set_writes_;

    public:
        VulkanDescriptorWriter(
            VulkanDescriptorSetLayout& descriptor_set_layout,
            VulkanDescriptorPool& descriptor_pool
            );
        
        VulkanDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorType descriptor_type, VkDescriptorBufferInfo* buffer_info);
        VulkanDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorType descriptor_type, VkDescriptorImageInfo* image_info);

        bool Build(VkDescriptorSet& descriptor_set);
        void Overwrite(VkDescriptorSet& descriptor_set);
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_WRITER