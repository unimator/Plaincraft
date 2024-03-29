﻿/*
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

#ifndef PLAINCRAFT_RENDER_ENGINE_PLAINCRAFT_RENDER_ENGINE
#define PLAINCRAFT_RENDER_ENGINE_PLAINCRAFT_RENDER_ENGINE

#include "../src/plaincraft/render_engine/render_engine.hpp"
#include "../src/plaincraft/render_engine/frame_config.hpp"

#include "../src/plaincraft/render_engine/texture/texture.hpp"
#include "../src/plaincraft/render_engine/texture/textures_factory.hpp"

#include "../src/plaincraft/render_engine/utils/image/image.hpp"
#include "../src/plaincraft/render_engine/camera/camera.hpp"

#include "../src/plaincraft/render_engine/gui/font/font_utils.hpp"
#include "../src/plaincraft/render_engine/gui/font/font.hpp"
#include "../src/plaincraft/render_engine/gui/font/fonts_factory.hpp"

#include "../src/plaincraft/render_engine/gui/menu/menu_button.hpp"
#include "../src/plaincraft/render_engine/gui/menu/menu_factory.hpp"
#include "../src/plaincraft/render_engine/gui/menu/menu.hpp"

#include "../src/plaincraft/render_engine/gui/gui_renderer.hpp"
#include "../src/plaincraft/render_engine/gui/gui_widget.hpp"

#include "../src/plaincraft/render_engine/scene/objects/cube.hpp"
#include "../src/plaincraft/render_engine/scene/objects/no_draw.hpp"
#include "../src/plaincraft/render_engine/scene/objects/mesh.hpp"
#include "../src/plaincraft/render_engine/scene/scene_renderer.hpp"
#include "../src/plaincraft/render_engine/scene/vertex.hpp"
#include "../src/plaincraft/render_engine/scene/mvp_matrix.hpp"
#include "../src/plaincraft/render_engine/scene/drawable.hpp"

#include "../src/plaincraft/render_engine/window/window.hpp"

#include "../src/plaincraft/render_engine/models/model.hpp"
#include "../src/plaincraft/render_engine/models/models_factory.hpp"

#endif // PLAINCRAFT_RENDER_ENGINE_PLAINCRAFT_RENDER_ENGINE