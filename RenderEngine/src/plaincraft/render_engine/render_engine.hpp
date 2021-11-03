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
#include <GLFW/glfw3.h>
#include "renderer/drawable.hpp"
#include "renderer/renderer.hpp"
#include "texture/textures_factory.hpp"
#include "texture/textures_repository.hpp"
#include "window/window.hpp"
#include "models/models_factory.hpp"

namespace plaincraft_render_engine {
	class RenderEngine {
	protected:
		std::shared_ptr<Window> window_;
		std::string title_ = "Plaincraft";

		std::shared_ptr<Camera> camera_;
		std::unique_ptr<Renderer> renderer_;
		std::vector<std::shared_ptr<Drawable>> drawables_list_;

		std::shared_ptr<TexturesRepository> textures_repository_;
		std::shared_ptr<TexturesFactory> textures_factory_;

		std::unique_ptr<ModelsFactory> models_factory_;

	public:
		virtual ~RenderEngine();
		
		auto GetCamera() const -> const std::shared_ptr<Camera>& { return camera_; }
		auto GetWindow() -> std::shared_ptr<Window> { return window_; }
		
		void AddDrawable(std::shared_ptr<Drawable> drawable);

		void GetCursorPosition(double* cursor_position_x, double* cursor_position_y);
		auto GetTexturesFactory() const -> std::shared_ptr<TexturesFactory> { return textures_factory_; }
		auto GetTexturesRepository() const -> std::shared_ptr<TexturesRepository> { return textures_repository_; }

		virtual void RenderFrame() = 0;

		auto GetModelsFactory() -> std::unique_ptr<ModelsFactory>& { return models_factory_; }
		
	protected:
		RenderEngine(std::shared_ptr<Window> window);

		RenderEngine(const RenderEngine& other) = delete;
		RenderEngine(RenderEngine&& other);

		RenderEngine& operator=(const RenderEngine& other) = default;		
		RenderEngine& operator=(RenderEngine&& other);
	};
}
#endif // PLAINCRAFT_RENDER_ENGINE_RENDER_ENGINE