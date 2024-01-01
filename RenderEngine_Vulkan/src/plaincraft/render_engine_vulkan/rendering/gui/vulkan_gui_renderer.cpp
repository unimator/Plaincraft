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

#include "vulkan_gui_renderer.hpp"
#include "../../utils/queue_family.hpp"
#include "windows/diagnostic/vulkan_diagnostic_widget.hpp"

namespace plaincraft_render_engine_vulkan
{
  VulkanGuiRenderer::VulkanGuiRenderer(const VulkanInstance &vulkan_instance,
                                     const VulkanDevice &vulkan_device,
                                     std::shared_ptr<VulkanWindow> vulkan_window,
                                     VkRenderPass render_pass)
      : VulkanGuiRenderer(vulkan_instance, vulkan_device, vulkan_window, render_pass, nullptr)
  {
  }

  VulkanGuiRenderer::VulkanGuiRenderer(const VulkanInstance &vulkan_instance,
                                     const VulkanDevice &vulkan_device,
                                     std::shared_ptr<VulkanWindow> vulkan_window,
                                     VkRenderPass render_pass,
                                     std::unique_ptr<VulkanGuiRenderer> old_context)
      : vulkan_device_(vulkan_device), vulkan_instance_(vulkan_instance), vulkan_window_(vulkan_window)
  {
    if (old_context != nullptr)
    {
      old_context = nullptr;
    }
    Initialize(render_pass);
    UploadFonts();

    debug_widgets_.push_back(std::make_unique<VulkanDiagnosticWidget>());
  }

  VulkanGuiRenderer::~VulkanGuiRenderer()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    auto &vulkan_device = vulkan_device_.get();
    if(imgui_descriptor_pool_ != VK_NULL_HANDLE)
    {
      vkDestroyDescriptorPool(vulkan_device.GetDevice(), imgui_descriptor_pool_, nullptr);
    }
  }

  void VulkanGuiRenderer::SetDebugWidgetsVisibility(bool are_debug_widgets_visible)
  {
    are_debug_widgets_visible_ = are_debug_widgets_visible;
  }

  void VulkanGuiRenderer::Render(VulkanRendererFrameConfig vulkan_renderer_frame_config)
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for(auto& widget : debug_widgets_)
    {
      widget->Draw(vulkan_renderer_frame_config.frame_config);
    }

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();

    ImGui_ImplVulkan_RenderDrawData(draw_data, vulkan_renderer_frame_config.command_buffer);
  }

  void VulkanGuiRenderer::Initialize(VkRenderPass render_pass)
  {
    auto &vulkan_device = vulkan_device_.get();
    auto &vulkan_instance = vulkan_instance_.get();

    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1 * std::size(pool_sizes);
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(vulkan_device.GetDevice(), &pool_info, nullptr, &imgui_descriptor_pool_) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create descriptor pool");
    }

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(vulkan_window_->GetInstance(), false);

    ImGui_ImplVulkan_InitInfo vulkan_init_info = {};
    vulkan_init_info.Instance = vulkan_instance.GetInstance();
    vulkan_init_info.PhysicalDevice = vulkan_device.GetPhysicalDevice();
    vulkan_init_info.Device = vulkan_device.GetDevice();
    vulkan_init_info.Queue = vulkan_device.GetTransferQueue();
    vulkan_init_info.DescriptorPool = imgui_descriptor_pool_;
    vulkan_init_info.MinImageCount = 3;
    vulkan_init_info.ImageCount = 3;
    vulkan_init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&vulkan_init_info, render_pass);
  }

  void VulkanGuiRenderer::UploadFonts()
  {
    auto &device = vulkan_device_.get();
    VkCommandBuffer command_buffer = device.BeginSingleTimeCommands(device.GetTransferCommandPool());

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    device.EndSingleTimeCommands(device.GetTransferCommandPool(), command_buffer, device.GetTransferQueue());
    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }
}