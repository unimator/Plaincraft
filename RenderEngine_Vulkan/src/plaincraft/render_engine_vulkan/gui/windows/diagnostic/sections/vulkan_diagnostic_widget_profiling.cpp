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

#include "./vulkan_diagnostic_widget_profiling.hpp"
#include <imgui.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <plaincraft_common.hpp>

namespace plaincraft_render_engine_vulkan 
{
    using namespace plaincraft_common;

    void VulkanDiagnosticWidgetProfiling::Render()
    {
        auto profiler = Profiler::GetInstance();
        auto &descriptions = profiler.GetDescriptions();

        ImGui::BeginTable("Diagnostics", 3, ImGuiTableFlags_None, {0, 0});
        for (auto &[name, description] : descriptions)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text(name.c_str());

            std::vector<float> plot_values;
            auto values = description.GetValues();
            float avarage_time = 0.0f;

            for (auto it = values.begin();
                 it != values.end();
                 ++it)
            {
                auto value = (*it).count();
                plot_values.push_back(value);
                avarage_time += value;
            }

            avarage_time /= plot_values.size();
            const char *format = "Avg: %f ms";
            size_t size = std::snprintf(nullptr, 0, format, avarage_time);
            std::vector<char> buffer(size + 1);
            std::snprintf(&buffer[0], buffer.size(), format, avarage_time);

            ImGui::TableNextColumn();
            ImVec2 plot_size = {500.0f, 0.0f};
            ImGui::PlotLines("time", plot_values.data(), plot_values.size(), 0, nullptr, FLT_MAX, FLT_MAX, plot_size);
            ImGui::TableNextColumn();
            ImGui::Text(buffer.data());
        }
        ImGui::EndTable();
    }
}