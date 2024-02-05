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
#include "scene/drawable.hpp"
#include "scene/scene_renderer.hpp"
#include "gui/menu/menu_factory.hpp"
#include "gui/font/fonts_factory.hpp"
#include "texture/textures_factory.hpp"
#include "texture/texture.hpp"
#include "window/window.hpp"
#include "models/models_factory.hpp"
#include "frame_config.hpp"
#include "gui/gui_renderer.hpp"
#include <mutex>

namespace plaincraft_render_engine {
	class RenderEngine {
	protected:
		std::shared_ptr<Window> window_;
		std::string title_ = "Plaincraft";

		std::shared_ptr<Camera> camera_;
		std::unique_ptr<SceneRenderer> scene_renderer_;
		std::unique_ptr<GuiRenderer> gui_renderer_;

		std::vector<std::shared_ptr<Drawable>> drawables_list_;
		std::mutex drawables_list_mutex_;

		std::vector<std::shared_ptr<GuiWidget>> widgets_list_;
		std::mutex widgets_list_mutex_;

		std::shared_ptr<TexturesFactory> textures_factory_;
		std::shared_ptr<ModelsFactory> models_factory_;
		std::shared_ptr<MenuFactory> menu_factory_;
		std::shared_ptr<FontsFactory> fonts_factory_;

	public:
		virtual ~RenderEngine();
		
		auto GetCamera() const -> const std::shared_ptr<Camera>& { return camera_; }
		auto GetWindow() -> std::shared_ptr<Window> { return window_; }

		void AddDrawable(std::shared_ptr<Drawable> drawable_to_add);
		void RemoveDrawable(std::shared_ptr<Drawable> drawable_to_remove);

		void AddWidget(std::shared_ptr<GuiWidget> widget_to_add);
		void RemoveWidget(std::shared_ptr<GuiWidget> widget_to_remove);
		
		void GetCursorPosition(double* cursor_position_x, double* cursor_position_y);


		plaincraft_common::Cache<Texture>& GetTexturesRepository();
		plaincraft_common::Cache<Font>& GetFontsRepository();

		std::shared_ptr<TexturesFactory> GetTexturesFactory();
		std::shared_ptr<ModelsFactory> GetModelsFactory();
		std::shared_ptr<MenuFactory> GetMenuFactory();
		std::shared_ptr<FontsFactory> GetFontsFactory();
		
		virtual void RenderFrame(const FrameConfig& frame_config) = 0;

	protected:
		RenderEngine(std::shared_ptr<Window> window);

		RenderEngine(const RenderEngine& other) = delete;
		RenderEngine(RenderEngine&& other);

		RenderEngine& operator=(const RenderEngine& other) = default;		
		RenderEngine& operator=(RenderEngine&& other);
	};
}
#endif // PLAINCRAFT_RENDER_ENGINE_RENDER_ENGINE