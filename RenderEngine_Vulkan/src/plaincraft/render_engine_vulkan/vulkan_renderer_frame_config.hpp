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

#ifndef VULKAN_RENDER_ENGINE_VULKAN_VULKAN_RENDERER_FRAME_CONFIG
#define VULKAN_RENDER_ENGINE_VULKAN_VULKAN_RENDERER_FRAME_CONFIG

#include <vulkan/vulkan.h>
#include <plaincraft_render_engine.hpp>

namespace plaincraft_render_engine_vulkan {
    using namespace plaincraft_render_engine;

    struct VulkanRendererFrameConfig {
        const FrameConfig& frame_config; 
        VkCommandBuffer& command_buffer;
        size_t image_index;
    };
};

#endif // VULKAN_RENDER_ENGINE_VULKAN_VULKAN_RENDERER_FRAME_CONFIG