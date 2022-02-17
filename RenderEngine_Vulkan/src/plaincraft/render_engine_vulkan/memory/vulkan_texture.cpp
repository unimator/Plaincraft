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

#include "vulkan_texture.hpp"

namespace plaincraft_render_engine_vulkan
{
	VulkanTexture::VulkanTexture(const VulkanDevice &device, const Image &image)
		: VulkanImage(device,
					  image.width,
					  image.height,
					  VK_FORMAT_R8G8B8A8_SRGB,
					  VK_IMAGE_TILING_OPTIMAL,
					  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
		texture_image_view_(device, image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT)
	{
		CreateTexture(image.data.get());
		CreateSampler();
	}

	VulkanTexture::~VulkanTexture()
	{
	}

	VkSampler VulkanTexture::GetSampler() const
	{
		return texture_sampler_;
	}

	const VulkanImageView& VulkanTexture::GetImageView() const 
	{
		return texture_image_view_;
	}

	void VulkanTexture::TransitionImageLayout(VkFormat format, VkImageLayout old_image_layout, VkImageLayout new_image_layout)
	{
		auto &device = device_.get();
		auto command_buffer = device.BeginSingleTimeCommands();

		VkImageMemoryBarrier image_memory_barrier{};
		image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_memory_barrier.oldLayout = old_image_layout;
		image_memory_barrier.newLayout = new_image_layout;
		image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.image = image_;
		image_memory_barrier.subresourceRange.baseMipLevel = 0;
		image_memory_barrier.subresourceRange.levelCount = 1;
		image_memory_barrier.subresourceRange.baseArrayLayer = 0;
		image_memory_barrier.subresourceRange.layerCount = 1;
		image_memory_barrier.srcAccessMask = 0;
		image_memory_barrier.dstAccessMask = 0;

		if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (device.HasStencilComponent(format))
			{
				image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkPipelineStageFlags source_stage;
		VkPipelineStageFlags destination_stage;

		if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			throw std::invalid_argument("Unsupported layout transition");
		}

		vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

		device.EndSingleTimeCommands(command_buffer);
	}

	void VulkanTexture::CreateTexture(void *image_data)
	{
		VkDeviceSize image_size = width_ * height_ * 4;
		VulkanBuffer staging_buffer(device_, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		staging_buffer.Map(image_size, 0);
		memcpy(staging_buffer.GetMappedData(), image_data, static_cast<size_t>(image_size));
		staging_buffer.Unmap();

		TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(staging_buffer.GetBuffer(), width_, height_);
		TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void VulkanTexture::CreateSampler()
	{
		auto &device = device_.get();
		VkPhysicalDeviceProperties physical_device_properties{};
		vkGetPhysicalDeviceProperties(device.GetPhysicalDevice(), &physical_device_properties);

		VkSamplerCreateInfo sampler_info{};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_NEAREST;
		sampler_info.minFilter = VK_FILTER_NEAREST;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 0; //physical_device_properties.limits.maxSamplerAnisotropy;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		if (vkCreateSampler(device.GetDevice(), &sampler_info, nullptr, &texture_sampler_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture sampler");
		}
	}
}