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
#include "../memory/vulkan_texture.hpp"
#include <cstring>

namespace plaincraft_render_engine_vulkan
{
    VulkanModel::VulkanModel(const VulkanDevice &device, std::shared_ptr<Mesh const> mesh)
        : Model(mesh),
          device_(device),
          vertex_buffer_(
              VulkanBuffer::MoveBuffer(device,
                                       VulkanBuffer::CreateFromVector(
                                           device,
                                           mesh->GetVertices(),
                                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
          index_buffer_(
              VulkanBuffer::MoveBuffer(device,
                                       VulkanBuffer::CreateFromVector(
                                           device,
                                           mesh->GetIndices(),
                                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
    {
    }

    
    VulkanModel::VulkanModel(VulkanModel&& other)
    :
        device_(other.device_),
        vertex_buffer_(std::move(other.vertex_buffer_)),
        index_buffer_(std::move(other.index_buffer_))
    {
    }

    VulkanModel& VulkanModel::operator=(VulkanModel&& other)
    {
        if(&other == this)
        {
            return *this;
        }

        this->index_buffer_ = std::move(other.index_buffer_);
        this->vertex_buffer_ = std::move(other.vertex_buffer_);

        return *this;
    }

    VulkanModel::~VulkanModel()
    {
    }

    void VulkanModel::Bind(VkCommandBuffer command_buffer)
    {
        VkBuffer buffers[] = {vertex_buffer_.GetBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(command_buffer, index_buffer_.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanModel::Draw(VkCommandBuffer command_buffer)
    {
        //vkCmdDraw(command_buffer, vertex_buffer_.GetInstanceCount(), 1, 0, 0);
        vkCmdDrawIndexed(command_buffer, index_buffer_.GetInstanceCount(), 1, 0, 0, 0);
    }
}