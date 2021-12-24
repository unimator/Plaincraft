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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_GUI_CONTEXT
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_GUI_CONTEXT

#include "../instance/vulkan_instance.hpp"
#include "../device/vulkan_device.hpp"
#include "../scene_rendering/vulkan_drawable.hpp"
#include "../window/vulkan_window.hpp"

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace plaincraft_render_engine_vulkan
{
    class VulkanGuiContext final : public VulkanDrawable
    {
    private:
        std::reference_wrapper<const VulkanDevice> vulkan_device_;
        std::reference_wrapper<const VulkanInstance> vulkan_instance_;
        std::shared_ptr<VulkanWindow> vulkan_window_;
        ImGui_ImplVulkanH_Window imgui_window_;

        VkDescriptorPool imgui_descriptor_pool_;

    public:
        VulkanGuiContext(const VulkanInstance &vulkan_instance,
                         const VulkanDevice &vulkan_device,
                         std::shared_ptr<VulkanWindow> vulkan_window,
                         VkRenderPass render_pass,
                         VkSurfaceKHR surface);
        VulkanGuiContext(const VulkanInstance &vulkan_instance,
                         const VulkanDevice &vulkan_device,
                         std::shared_ptr<VulkanWindow> vulkan_window,
                         VkRenderPass render_pass,
                         VkSurfaceKHR surface,
                         std::unique_ptr<VulkanGuiContext> old_context);

        VulkanGuiContext(const VulkanGuiContext& other) = delete;
        VulkanGuiContext& operator=(const VulkanGuiContext& other) = delete;
        ~VulkanGuiContext();

        void Bind(VkCommandBuffer command_buffer) override;
        void Draw(VkCommandBuffer command_buffer) override;

    private:
        void Initialize(VkRenderPass render_pass);
        void UploadFonts();
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_GUI_CONTEXT