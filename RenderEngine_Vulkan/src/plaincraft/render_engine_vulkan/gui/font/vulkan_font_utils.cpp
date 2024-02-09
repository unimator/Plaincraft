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

#include "vulkan_font_utils.hpp"
#include "vulkan_font.hpp"
#include <imgui.h>

namespace plaincraft_render_engine_vulkan
{
    float VulkanFontUtils::CalcStringWidth(std::string text, std::shared_ptr<plaincraft_render_engine::Font> font)
    {
        auto font_im = std::static_pointer_cast<VulkanFont>(font)->GetImFont();

        ImGui::PushFont(font_im);
        auto result = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::PopFont();

        return result;
    }

    float VulkanFontUtils::CalcStringHeight(std::string text, std::shared_ptr<plaincraft_render_engine::Font> font)
    {
        auto font_im = std::static_pointer_cast<VulkanFont>(font)->GetImFont();

        ImGui::PushFont(font_im);
        auto result = ImGui::CalcTextSize(text.c_str()).y;
        ImGui::PopFont();

        return result;
    }
}