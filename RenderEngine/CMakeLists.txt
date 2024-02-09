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

add_library("RenderEngine" "")

target_sources("RenderEngine"
PRIVATE
    src/plaincraft/render_engine/camera/camera.cpp
    src/plaincraft/render_engine/events/window_events_handler.cpp
    src/plaincraft/render_engine/gui/font/fonts_factory.cpp
    src/plaincraft/render_engine/gui/menu/menu_button.cpp
    src/plaincraft/render_engine/gui/menu/menu_factory.cpp
    src/plaincraft/render_engine/gui/menu/menu.cpp
    src/plaincraft/render_engine/gui/gui_renderer.cpp
    src/plaincraft/render_engine/gui/gui_widget.cpp
    src/plaincraft/render_engine/models/model.cpp
    src/plaincraft/render_engine/models/models_factory.cpp
    src/plaincraft/render_engine/scene/objects/cube.cpp
    src/plaincraft/render_engine/scene/objects/mesh.cpp
    src/plaincraft/render_engine/scene/objects/no_draw.cpp
    src/plaincraft/render_engine/scene/drawable.cpp
    src/plaincraft/render_engine/scene/scene_renderer.cpp
    src/plaincraft/render_engine/scene/vertex.cpp
    src/plaincraft/render_engine/texture/textures_repository.cpp
    src/plaincraft/render_engine/utils/image/image.cpp
    src/plaincraft/render_engine/window/window.cpp
    src/plaincraft/render_engine/render_engine.cpp
)

target_include_directories("RenderEngine" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries("RenderEngine" PRIVATE "Common")
