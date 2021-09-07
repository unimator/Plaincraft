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

#include "vulkan_model.hpp"
#include <cstring>

namespace plaincraft_render_engine_vulkan {
    VulkanModel::VulkanModel(VulkanDevice& device, std::shared_ptr<Polygon const> polygon) 
        : device_(device), buffer_manager_(VulkanBufferManager(device)) {
        CreateVertexBuffer(polygon->GetVertices());
        CreateIndexBuffer(polygon->GetIndices());
    }

    VulkanModel::~VulkanModel() {
        vkDestroyBuffer(device_.GetDevice(), vertex_buffer_, nullptr);
        vkFreeMemory(device_.GetDevice(), vertex_buffer_memory_, nullptr);
    }

    void VulkanModel::Bind(VkCommandBuffer command_buffer)
    {
        VkBuffer buffers[] = {vertex_buffer_};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

        vkCmdBindIndexBuffer(command_buffer, index_buffer_, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanModel::Draw(VkCommandBuffer command_buffer)
    {
        //vkCmdDraw(command_buffer, vertex_count_, 1, 0, 0);
        vkCmdDrawIndexed(command_buffer, index_count_, 1, 0, 0, 0);
    }

    void VulkanModel::CreateVertexBuffer(const std::vector<Vertex>& vertices)
    {
        vertex_count_ = static_cast<uint32_t>(vertices.size());

        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count_;

        buffer_manager_.CreateBuffer(
            buffer_size, 
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertex_buffer_,
            vertex_buffer_memory_
        );

        void* data;
        vkMapMemory(device_.GetDevice(), vertex_buffer_memory_, 0, buffer_size, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
        vkUnmapMemory(device_.GetDevice(), vertex_buffer_memory_);
    }

    void VulkanModel::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        index_count_ = static_cast<uint32_t>(indices.size());

        VkDeviceSize buffer_size = sizeof(indices[0]) * index_count_;

        buffer_manager_.CreateBuffer(
            buffer_size, 
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            index_buffer_,
            index_buffer_memory_
        );

        void* data;
        vkMapMemory(device_.GetDevice(), index_buffer_memory_, 0, buffer_size, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
        vkUnmapMemory(device_.GetDevice(), index_buffer_memory_);    
        }
}