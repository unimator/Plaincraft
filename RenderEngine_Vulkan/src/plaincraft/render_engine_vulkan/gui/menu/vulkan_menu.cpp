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

#include "vulkan_menu.hpp"
#include "../font/vulkan_font.hpp"

namespace plaincraft_render_engine_vulkan
{
    VulkanMenu::VulkanMenu() {}

    VulkanMenu::~VulkanMenu()
    {
    }

    void VulkanMenu::Draw(const FrameConfig &frame_config)
    {
        bool is_visible;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos({static_cast<float>(GetPositionX()), static_cast<float>(GetPositionY())});
        ImGui::SetNextWindowSize({static_cast<float>(GetWidth()), static_cast<float>(GetHeight())});
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushFont(std::static_pointer_cast<VulkanFont>(GetFont())->GetImFont());
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        ImGui::Begin("Menu", &is_visible, window_flags);
        for (auto &button : GetButtons())
        {
            ImGui::Button(button->GetName().c_str());
        }
        ImGui::End();
        ImGui::PopFont();
        ImGui::PopStyleVar();
    }
}