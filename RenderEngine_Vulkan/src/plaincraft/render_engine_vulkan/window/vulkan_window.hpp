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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_WINDOW
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_WINDOW

#include "../common.hpp"
#include <plaincraft_render_engine.hpp>
#include <vulkan/vulkan.h>

namespace plaincraft_render_engine_vulkan
{
    using namespace plaincraft_render_engine;
    class VulkanWindow final : public Window 
    {
    private:
        bool was_resized_ = false;

    public:
        VulkanWindow(std::string title, uint32_t width, uint32_t height);

        VulkanWindow(const VulkanWindow& other) = delete;
        VulkanWindow(VulkanWindow&& other);
        VulkanWindow& operator=(const VulkanWindow& other) = delete;
        VulkanWindow& operator=(VulkanWindow&& other);

        VkSurfaceKHR CreateSurface(VkInstance instance);

        auto WasResized() -> bool const {return was_resized_;}
        void ResetWasResized() {was_resized_ = false;}  

    private:
        void Initialize();

        static void FramebufferResizeCallback(GLFWwindow* instance, int width, int height);
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_WINDOW
