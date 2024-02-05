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

#include "../utils/queue_family.hpp"
#include "windows/diagnostic/vulkan_diagnostic_widget.hpp"
#include "menu/in_game/vulkan_in_game_menu.hpp"
#include "vulkan_gui_renderer.hpp"

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
      widgets_list_ = std::move(old_context->widgets_list_);
      old_context = nullptr;
    }
    Initialize(render_pass);
    UploadFonts();
  }

  VulkanGuiRenderer::~VulkanGuiRenderer()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    auto &vulkan_device = vulkan_device_.get();
    if (imgui_descriptor_pool_ != VK_NULL_HANDLE)
    {
      vkDestroyDescriptorPool(vulkan_device.GetDevice(), imgui_descriptor_pool_, nullptr);
    }
  }

  void VulkanGuiRenderer::BeginFrame(VulkanRendererFrameConfig& frame_config)
  {
    if (frame_config_ != nullptr && frame_config_ != &frame_config)
		{
			throw std::runtime_error("Other frame has already begun recording");
		}

		frame_config_ = &frame_config;
  }

  void VulkanGuiRenderer::EndFrame()
  {
    frame_config_ = nullptr;
  }

  void VulkanGuiRenderer::Render()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    for (auto &widget : widgets_list_)
    {
      ImGui::PushFont(details_font_.get());
      widget->Draw(frame_config_->frame_config);
      ImGui::PopFont();
    }

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();

    ImGui_ImplVulkan_RenderDrawData(draw_data, frame_config_->command_buffer);
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

    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig standard_font_config {};
    standard_font_config.SizePixels = 42;
    standard_font_ = std::shared_ptr<ImFont>(io.Fonts->AddFontDefault(&standard_font_config));

    ImFontConfig details_font_config {};
    details_font_config.SizePixels = 12;
    details_font_ = std::shared_ptr<ImFont>(io.Fonts->AddFontDefault(&details_font_config));

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    device.EndSingleTimeCommands(device.GetTransferCommandPool(), command_buffer, device.GetTransferQueue());
    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }
}