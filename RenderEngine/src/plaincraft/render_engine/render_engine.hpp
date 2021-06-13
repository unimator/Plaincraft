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

#ifndef PLAINCRAFT_RENDER_ENGINE_RENDER_ENGINE
#define PLAINCRAFT_RENDER_ENGINE_RENDER_ENGINE

#include "common.hpp"
#include <GLFW\glfw3.h>
#include "renderer\drawable.hpp"
#include "renderer\renderer.hpp"
#include "shader\shaders_repository.hpp"
#include "texture\textures_factory.hpp"
#include "texture\textures_repository.hpp"

namespace plaincraft_render_engine {
	class DLLEXPORT_PLAINCRAFT_RENDER_ENGINE RenderEngine {
	protected:
		GLFWwindow* window_;
		uint32_t width_ = 1024, height_ = 768;
		std::string title_ = "Plaincraft";

		std::shared_ptr<Camera> camera_;
		std::shared_ptr<Renderer> renderer_;
		std::vector<std::shared_ptr<Drawable>> drawables_list_;

		std::shared_ptr<ShadersRepository> shaders_repository_;

		std::shared_ptr<TexturesRepository> textures_repository_;
		std::shared_ptr<TexturesFactory> textures_factory_;

	public:
		RenderEngine(uint32_t width, uint32_t height);

		virtual ~RenderEngine();

		auto GetWindow() -> GLFWwindow* { return window_; }

		auto GetWidth() -> uint32_t { return width_; }
		auto GetHeight() -> uint32_t { return height_; }

		virtual void RenderFrame() = 0;
		void GetCursorPosition(double* cursor_position_x, double* cursor_position_y);

		auto GetCamera() const -> const std::shared_ptr<Camera>& { return camera_; }

		void AddDrawable(std::shared_ptr<Drawable> drawable);

		virtual std::unique_ptr<Shader> CreateDefaultShader() = 0;

		auto GetShadersRepository() const -> std::shared_ptr<ShadersRepository> { return shaders_repository_; }

		auto GetTexturesFactory() const -> std::shared_ptr<TexturesFactory> { return textures_factory_; }
		auto GetTexturesRepository() const -> std::shared_ptr<TexturesRepository> { return textures_repository_; }
	};
}
#endif // PLAINCRAFT_RENDER_ENGINE_RENDER_ENGINE