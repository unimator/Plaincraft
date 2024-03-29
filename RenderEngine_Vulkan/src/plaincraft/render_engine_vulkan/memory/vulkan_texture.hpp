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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_TEXTURE
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_TEXTURE

#include "vulkan_image.hpp"
#include "vulkan_image_view.hpp"
#include "../device/vulkan_device.hpp"
#include "vulkan_buffer.hpp"
#include <plaincraft_render_engine.hpp>
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan
{
    using namespace plaincraft_render_engine;

    class VulkanTexture : public Texture, public VulkanImage
    { 
    private:
        VkSampler texture_sampler_;
        VulkanImageView texture_image_view_;

    public:
        VulkanTexture(const VulkanDevice& device, const Image& image);

        VulkanTexture(const VulkanTexture& other) = delete;
        VulkanTexture& operator=(const VulkanTexture& other) = delete;

        virtual ~VulkanTexture() override;

        VkSampler GetSampler() const;
        const VulkanImageView& GetImageView() const;

    private:
        void TransitionImageLayout(VkFormat format, VkImageLayout old_image_layout, VkImageLayout new_image_layout);

        void CreateTexture(void* image_data);
        void CreateSampler();
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_TEXTURE