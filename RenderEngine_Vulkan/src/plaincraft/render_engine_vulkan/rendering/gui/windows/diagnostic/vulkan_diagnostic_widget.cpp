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
#include <plaincraft_common.hpp>

namespace plaincraft_render_engine_vulkan
{
    using namespace plaincraft_common;

    VulkanDiagnosticWidget::VulkanDiagnosticWidget()
    {

    }

    void VulkanDiagnosticWidget::Draw()
    {
        if(!is_visible_)
        {
            return;
        }

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGuiWindowFlags window_flags = 0;
        if (!ImGui::Begin("Diagnostic", &is_visible_, window_flags))
        {
            ImGui::End();
            return;
        }

        auto profiler = Profiler::GetInstance();
        auto& descriptions = profiler.GetDescriptions();

        for(auto& description : descriptions)
        {
            if(ImGui::TreeNode(description.second.GetName().c_str()))
            {
                std::vector<float> values;
                for(auto& value : description.second.GetValues())
                {
                    values.push_back(value.count());
                }

                //PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
                ImGui::PlotLines("time", values.data(), profiling_history_length, 0, nullptr, 0, 10);
                ImGui::TreePop();
            }
        }

        

        ImGui::End();
    }
}