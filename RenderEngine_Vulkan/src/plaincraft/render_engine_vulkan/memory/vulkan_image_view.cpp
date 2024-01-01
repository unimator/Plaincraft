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

#include "vulkan_image_view.hpp"

namespace plaincraft_render_engine_vulkan
{
    VulkanImageView::VulkanImageView(const VulkanDevice &device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
        : device_(device), format_(format), aspect_flags_(aspect_flags)
    {
        CreateImageView(image);
    }

    VulkanImageView::VulkanImageView(VulkanImageView&& other) noexcept
        : device_(other.device_), image_view_(other.image_view_), aspect_flags_(other.aspect_flags_)
    {
        other.image_view_ = nullptr;
    }

    VulkanImageView& VulkanImageView::operator=(VulkanImageView&& other) noexcept
    {
        if(this == &other) {
            return *this;
        }

        this->image_view_ = other.image_view_;
        this->aspect_flags_ = other.aspect_flags_;
        this->device_ = other.device_;

        other.image_view_ = VK_NULL_HANDLE;
        other.aspect_flags_ = 0;

        return *this;
    }

    VulkanImageView::~VulkanImageView() 
    {
        const auto& device = device_.get();
        vkDestroyImageView(device.GetDevice(), image_view_, nullptr);
    }

    VkImageView VulkanImageView::GetImageView() const
    {
        return image_view_;
    }

    void VulkanImageView::CreateImageView(VkImage image)
    {
        auto device = device_.get().GetDevice();
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = image;
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = format_;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = aspect_flags_;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &create_info, nullptr, &image_view_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image views");
        }
    }

}