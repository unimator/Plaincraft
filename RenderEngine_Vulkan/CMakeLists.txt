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

add_library("RenderEngine_Vulkan" "")

target_sources("RenderEngine_Vulkan"
PRIVATE
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_pool.cpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_set_layout.cpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_writer.cpp
    src/plaincraft/render_engine_vulkan/device/vulkan_device.cpp
    src/plaincraft/render_engine_vulkan/gui/font/vulkan_font_utils.cpp
    src/plaincraft/render_engine_vulkan/gui/font/vulkan_font.cpp
    src/plaincraft/render_engine_vulkan/gui/font/vulkan_fonts_factory.cpp
    src/plaincraft/render_engine_vulkan/gui/menu/vulkan_menu_factory.cpp
    src/plaincraft/render_engine_vulkan/gui/menu/vulkan_menu.cpp
    src/plaincraft/render_engine_vulkan/gui/windows/diagnostic/sections/vulkan_diagnostic_widget_logger.cpp
    src/plaincraft/render_engine_vulkan/gui/windows/diagnostic/sections/vulkan_diagnostic_widget_profiling.cpp
    src/plaincraft/render_engine_vulkan/gui/windows/diagnostic/sections/vulkan_diagnostic_widget_section.cpp
    src/plaincraft/render_engine_vulkan/gui/windows/diagnostic/vulkan_diagnostic_widget.cpp
    src/plaincraft/render_engine_vulkan/gui/vulkan_gui_renderer.cpp
    src/plaincraft/render_engine_vulkan/instance/vulkan_instance_config.cpp
    src/plaincraft/render_engine_vulkan/instance/vulkan_instance.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_buffer.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_image_view.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_image.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_texture.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_uniform_buffer.cpp
    src/plaincraft/render_engine_vulkan/models/vulkan_model.cpp
    src/plaincraft/render_engine_vulkan/models/vulkan_models_factory.cpp
    src/plaincraft/render_engine_vulkan/pipeline/vulkan_pipeline.cpp
    src/plaincraft/render_engine_vulkan/scene/vertex_utils.cpp
    src/plaincraft/render_engine_vulkan/scene/vulkan_scene_renderer.cpp
    src/plaincraft/render_engine_vulkan/shader/vulkan_shader.cpp
    src/plaincraft/render_engine_vulkan/swapchain/vulkan_swapchain.cpp
    src/plaincraft/render_engine_vulkan/textures/vulkan_textures_factory.cpp
    src/plaincraft/render_engine_vulkan/utils/queue_family.cpp
    src/plaincraft/render_engine_vulkan/window/vulkan_window.cpp
    src/plaincraft/render_engine_vulkan/vulkan_render_engine.cpp
)

find_package(Vulkan)

target_include_directories("RenderEngine_Vulkan" PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories("RenderEngine_Vulkan" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries("RenderEngine_Vulkan" PRIVATE "Common" "RenderEngine" "Dear_ImGui_Vulkan")
target_link_libraries("RenderEngine_Vulkan" PUBLIC ${Vulkan_LIBRARIES})
