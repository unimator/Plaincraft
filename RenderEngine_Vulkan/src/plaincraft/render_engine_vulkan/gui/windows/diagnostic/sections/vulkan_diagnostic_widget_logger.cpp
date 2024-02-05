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

#include "./vulkan_diagnostic_widget_logger.hpp"
#include <imgui.h>
#include <plaincraft_common.hpp>

namespace plaincraft_render_engine_vulkan
{
    using namespace plaincraft_common;

    void VulkanDiagnosticWidgetLogger::Render()
    {
        auto& log_values = Logger::GetValues();
        ImGui::BeginTable("LogValues", 2, ImGuiTabBarFlags_None, {0, 0});
        {
            for(auto &[key, value] : log_values)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text(key.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(value.c_str());

            }
        }
        ImGui::EndTable();
    }
}