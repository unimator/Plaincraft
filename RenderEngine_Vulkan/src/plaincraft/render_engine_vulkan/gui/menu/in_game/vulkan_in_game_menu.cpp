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

#include "vulkan_in_game_menu.hpp"
#include <cstdio>

namespace plaincraft_render_engine_vulkan
{
    VulkanInGameMenu::VulkanInGameMenu(std::shared_ptr<ImFont> standard_font)
        : standard_font_(standard_font)
    {
    }

    void VulkanInGameMenu::Draw(const FrameConfig& frame_config)
    {
        constexpr float padding = 10.0f;

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        const auto viewport_size = viewport->WorkSize;

        bool is_visible;

        ImVec2 window_position = {};
        window_position.x = padding;
        window_position.y = viewport_size.y - (padding + 200);
        ImGui::SetNextWindowPos(window_position);
        ImGui::SetNextWindowSize({200, 200});
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::PushFont(standard_font_.get());
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        ImGui::Begin("Menu", &is_visible, window_flags);
        bool clicked = ImGui::Button("Quit");
        ImGui::End();
        ImGui::PopFont();
        ImGui::PopStyleVar();

    }
}