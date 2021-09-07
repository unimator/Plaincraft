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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_MODEL
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_MODEL

#include "../device/vulkan_device.hpp"
#include "../memory/vulkan_buffer_manager.hpp"
#include <plaincraft_render_engine.hpp>
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan {
    using namespace plaincraft_render_engine;
    
    class VulkanModel : Model {
    private:
        VulkanDevice& device_;

        VkBuffer vertex_buffer_;
        VkDeviceMemory vertex_buffer_memory_;
        uint32_t vertex_count_;

        VkBuffer index_buffer_;
        VkDeviceMemory index_buffer_memory_;
        uint32_t index_count_;

        VulkanBufferManager& buffer_manager_;
    
    public:
        VulkanModel(VulkanDevice& device, std::shared_ptr<Polygon const> polygon);
        ~VulkanModel();

        VulkanModel(const VulkanModel& other) = delete;
        VulkanModel& operator=(const VulkanModel& other) = delete;

        void Bind(VkCommandBuffer command_buffer);
        void Draw(VkCommandBuffer command_buffer);

    private:
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_MODEL