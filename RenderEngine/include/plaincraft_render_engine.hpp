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

#ifndef PLAINCRAFT_RENDER_ENGINE_PLAINCRAFT_RENDER_ENGINE
#define PLAINCRAFT_RENDER_ENGINE_PLAINCRAFT_RENDER_ENGINE

#include "../src/plaincraft/render_engine/render_engine.hpp"

#include "../src/plaincraft/render_engine/texture/texture.hpp"
#include "../src/plaincraft/render_engine/texture/textures_repository.hpp"
#include "../src/plaincraft/render_engine/texture/textures_factory.hpp"

#include "../src/plaincraft/render_engine/utils/image/image.hpp"
#include "../src/plaincraft/render_engine/camera/camera.hpp"

#include "../src/plaincraft/render_engine/renderer/renderer.hpp"
#include "../src/plaincraft/render_engine/renderer/model.hpp"
#include "../src/plaincraft/render_engine/renderer/vertex.hpp"
#include "../src/plaincraft/render_engine/renderer/mvp_matrix.hpp"

#include "../src/plaincraft/render_engine/renderer/drawable.hpp"
#include "../src/plaincraft/render_engine/renderer/objects/cube.hpp"
#include "../src/plaincraft/render_engine/renderer/objects/no_draw.hpp"
#include "../src/plaincraft/render_engine/renderer/objects/mesh.hpp"

#include "../src/plaincraft/render_engine/window/window.hpp"

#endif // PLAINCRAFT_RENDER_ENGINE_PLAINCRAFT_RENDER_ENGINE