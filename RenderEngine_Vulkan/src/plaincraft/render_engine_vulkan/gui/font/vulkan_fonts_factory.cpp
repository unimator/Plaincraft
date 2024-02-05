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

#include "vulkan_fonts_factory.hpp"
#include "vulkan_font.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace plaincraft_render_engine_vulkan
{
    VulkanFontsFactory::VulkanFontsFactory(const VulkanDevice &vulkan_device)
        : vulkan_device_(vulkan_device)
    {
    }

    std::vector<std::pair<std::string, std::shared_ptr<plaincraft_render_engine::Font>>> VulkanFontsFactory::LoadStandardFonts()
    {
        std::vector<std::pair<std::string, std::shared_ptr<plaincraft_render_engine::Font>>> result;

        auto &device = vulkan_device_.get();
        VkCommandBuffer command_buffer = device.BeginSingleTimeCommands(device.GetTransferCommandPool());

        ImGuiIO &io = ImGui::GetIO();
        ImFontConfig standard_font_config{};
        standard_font_config.SizePixels = 42;
        auto standard_font = std::make_shared<VulkanFont>(io.Fonts->AddFontDefault(&standard_font_config));
        result.push_back(std::pair<std::string, std::shared_ptr<VulkanFont>>("standard", standard_font));

        ImFontConfig details_font_config{};
        details_font_config.SizePixels = 12;
        auto details_font = std::make_shared<VulkanFont>(io.Fonts->AddFontDefault(&details_font_config));
        result.push_back(std::pair<std::string, std::shared_ptr<VulkanFont>>("details", details_font));

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        device.EndSingleTimeCommands(device.GetTransferCommandPool(), command_buffer, device.GetTransferQueue());
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        return result;
    }
}