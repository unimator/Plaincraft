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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_POOL
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_POOL

#include "../device/vulkan_device.hpp"
#include <vector>
#include <memory>

namespace plaincraft_render_engine_vulkan
{
    class VulkanDescriptorWriter;

    class VulkanDescriptorPool
    {
    public:
        class Builder
        {
        private:
            VulkanDevice &device_;
            std::vector<VkDescriptorPoolSize> descriptor_pool_sizes_{};
            uint32_t max_sets_ = 1000;
            VkDescriptorPoolCreateFlags descriptor_pool_flags_ = 0;

        public:
            Builder(VulkanDevice &device);

            Builder &AddPoolSize(VkDescriptorType descriptor_type, uint32_t count);
            Builder &SetPoolFlags(VkDescriptorPoolCreateFlags pdescriptor_pool_flags);
            Builder &SetMaxSets(uint32_t count);
            std::unique_ptr<VulkanDescriptorPool> Build() const;
        };

    private:
        VulkanDevice &device_;
        VkDescriptorPool descriptor_pool_;

        friend class VulkanDescriptorWriter;

    public:
        VulkanDescriptorPool(
            VulkanDevice &device,
            uint32_t max_sets,
            VkDescriptorPoolCreateFlags descriptor_pool_flags,
            const std::vector<VkDescriptorPoolSize> &pool_sizes);
        ~VulkanDescriptorPool();
        VulkanDescriptorPool(const VulkanDescriptorPool &other) = delete;
        VulkanDescriptorPool &operator=(const VulkanDescriptorPool &other) = delete;

        bool AllocateDescriptor(
            const VkDescriptorSetLayout descriptor_set_layout,
            VkDescriptorSet &descriptor_set) const;

        void FreeDescriptors(std::vector<VkDescriptorSet> &descriptor_sets) const;

        void ResetPool();
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_DESCRIPTOR_POOL