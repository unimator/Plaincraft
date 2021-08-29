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

#include "vulkan_buffer_manager.hpp"
#include <stdexcept>

namespace plaincraft_render_engine_vulkan {
    VulkanBufferManager::VulkanBufferManager(const VulkanDevice& device) 
        : device_(device) 
    {} 

    VkCommandBuffer VulkanBufferManager::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool = device_.GetCommandPool();
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(device_.GetDevice(), &allocate_info, &command_buffer);

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &begin_info);

		return command_buffer;
	}

	void VulkanBufferManager::EndSingleTimeCommands(VkCommandBuffer command_buffer)
	{
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		auto graphics_queue = device_.GetGraphicsQueue();

		vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queue);

		vkFreeCommandBuffers(device_.GetDevice(), device_.GetCommandPool(), 1, &command_buffer);
	}

	void VulkanBufferManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
    {
        VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto device = device_.GetDevice();

		if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vertex buffer");
		}

		VkMemoryRequirements memory_requirements;
		vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

		VkMemoryAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.allocationSize = memory_requirements.size;
		allocate_info.memoryTypeIndex = device_.FindMemoryType(memory_requirements.memoryTypeBits, memory_properties);

		if (vkAllocateMemory(device, &allocate_info, nullptr, &buffer_memory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate vertex buffer memory");
		}

		vkBindBufferMemory(device, buffer, buffer_memory, 0);
    }

	void VulkanBufferManager::CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer, VkDeviceSize size)
	{
		auto command_buffer = BeginSingleTimeCommands();

		VkBufferCopy copy_region{};
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;

		vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &copy_region);

		EndSingleTimeCommands(command_buffer);
	}
}