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

#include "vulkan_diagnostic_widget.hpp"
#include <imgui.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

namespace plaincraft_render_engine_vulkan
{
    VulkanDiagnosticWidget::VulkanDiagnosticWidget()
    {
        sections_.push_back(std::make_unique<VulkanDiagnosticWidgetProfiling>(std::move(VulkanDiagnosticWidgetProfiling())));
        sections_.push_back(std::make_unique<VulkanDiagnosticWidgetLogger>(std::move(VulkanDiagnosticWidgetLogger())));
    }

    void VulkanDiagnosticWidget::Draw()
    {
        constexpr float padding = 10.0f;

        if (!is_visible_)
        {
            return;
        }

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 window_position = {};
        window_position.x = padding;
        window_position.y = padding;
        ImGui::SetNextWindowPos(window_position);
        ImGui::SetNextWindowSize({viewport->WorkSize.x - 2 * padding, 0});
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (!ImGui::Begin("Diagnostic", &is_visible_, window_flags))
        {
            ImGui::End();
            return;
        }

        for(auto& section : sections_)
        {
            section->Render();
        }

        ImGui::End();
    }
}