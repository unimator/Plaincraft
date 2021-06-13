/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin G�rka

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

#include "render_engine.hpp"
#include <iostream>
#include <utility>

namespace plaincraft_render_engine {
	RenderEngine::RenderEngine(uint32_t width, uint32_t height) 
		: width_(width), height_(height), window_(nullptr), camera_(std::shared_ptr<Camera>(new Camera())) {
		shaders_repository_ = std::shared_ptr<ShadersRepository>(new ShadersRepository());
		textures_repository_ = std::shared_ptr<TexturesRepository>(new TexturesRepository());
	}

	RenderEngine::~RenderEngine() = default;

	void RenderEngine::GetCursorPosition(double* cursor_position_x, double* cursor_position_y) {
		glfwGetCursorPos(window_, cursor_position_x, cursor_position_y);
	}

	void RenderEngine::RenderFrame() {
		for (auto drawable : drawables_list_) {
			renderer_->Batch(drawable);
			renderer_->Render();
		}
	}

	void RenderEngine::AddDrawable(std::shared_ptr<Drawable> drawable) {
		if (!drawable->GetShader()) {
			drawable->SetShader(shaders_repository_->GetShader("default"));
		}
		drawables_list_.push_back(drawable);
	}
}