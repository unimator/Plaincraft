#[[
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
]]

add_library("Dear_ImGui_Vulkan" "")

target_sources("Dear_ImGui_Vulkan" 
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_widgets.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/backends/imgui_impl_vulkan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/backends/imgui_impl_glfw.cpp"
)
