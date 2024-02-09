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

#include "render_engine.hpp"
#include <iostream>
#include <utility>

namespace plaincraft_render_engine
{
	RenderEngine::RenderEngine(std::shared_ptr<Window> window)
		: window_(std::move(window)),
		  camera_(std::make_shared<Camera>())
	{
		camera_->direction = glm::vec3(0.0f, 0.0f, -1.0f);
		camera_->up = glm::vec3(0.0f, 1.0f, 0.0f);
		camera_->fov = 50.0f;
	}

	RenderEngine::~RenderEngine(){
		std::lock_guard guard(drawables_list_mutex_);
		drawables_list_.clear();
	};

	RenderEngine::RenderEngine(RenderEngine &&other)
		: window_(std::move(other.window_))
	{
		this->camera_ = std::move(other.camera_);
		this->scene_renderer_ = std::move(other.scene_renderer_);
		this->gui_renderer_ = std::move(other.gui_renderer_);

		std::lock_guard drawables_lg(drawables_list_mutex_);
		this->drawables_list_ = std::move(other.drawables_list_);

		std::lock_guard widgets_lg(widgets_list_mutex_);
		this->widgets_list_ = std::move(other.widgets_list_);
		
		this->textures_factory_ = std::move(other.textures_factory_);
		this->models_factory_ = std::move(other.models_factory_);
		this->menu_factory_ = std::move(other.menu_factory_);
		this->fonts_factory_ = std::move(other.fonts_factory_);
	}

	RenderEngine &RenderEngine::operator=(RenderEngine &&other)
	{
		this->camera_ = std::move(other.camera_);
		this->scene_renderer_ = std::move(other.scene_renderer_);
		this->gui_renderer_ = std::move(other.gui_renderer_);

		std::lock_guard drawables_lg(drawables_list_mutex_);
		this->drawables_list_ = std::move(other.drawables_list_);

		std::lock_guard widgets_lg(widgets_list_mutex_);
		this->widgets_list_ = std::move(other.widgets_list_);
		
		this->textures_factory_ = std::move(other.textures_factory_);
		this->models_factory_ = std::move(other.models_factory_);
		this->menu_factory_ = std::move(other.menu_factory_);
		this->fonts_factory_ = std::move(other.fonts_factory_);

		return *this;
	}

	void RenderEngine::GetCursorPosition(double *cursor_position_x, double *cursor_position_y)
	{
		glfwGetCursorPos(window_->GetInstance(), cursor_position_x, cursor_position_y);
	}

	std::unique_ptr<TexturesFactory>& RenderEngine::GetTexturesFactory()
	{
		return textures_factory_;
	}

	std::unique_ptr<ModelsFactory>& RenderEngine::GetModelsFactory()
	{
		return models_factory_;
	}

	std::unique_ptr<MenuFactory>& RenderEngine::GetMenuFactory()
	{
		return menu_factory_;
	}

	std::unique_ptr<FontsFactory>& RenderEngine::GetFontsFactory()
	{
		return fonts_factory_;
	}

	void RenderEngine::AddDrawable(std::shared_ptr<Drawable> drawable_to_add)
	{
		std::lock_guard guard(drawables_list_mutex_);
		drawables_list_.push_back(drawable_to_add);
	}

	void RenderEngine::RemoveDrawable(std::shared_ptr<Drawable> drawable_to_remove)
	{
		std::lock_guard guard(drawables_list_mutex_);
		drawables_list_.erase(std::remove_if(drawables_list_.begin(),
											 drawables_list_.end(),
											 [&](std::shared_ptr<Drawable> const &drawable)
											 {
												 return drawable == drawable_to_remove;
											 }),
							  drawables_list_.end());
	}

	void RenderEngine::AddWidget(std::shared_ptr<GuiWidget> widget_to_add)
	{
		std::lock_guard guard(widgets_list_mutex_);
		widgets_list_.push_back(widget_to_add);
	}

	void RenderEngine::RemoveWidget(std::shared_ptr<GuiWidget> widget_to_remove)
	{
		std::lock_guard guard(widgets_list_mutex_);
		widgets_list_.erase(std::remove_if(widgets_list_.begin(),
										   widgets_list_.end(),
										   [&](std::shared_ptr<GuiWidget> const &widget)
										   {
											   return widget == widget_to_remove;
										   }),
							widgets_list_.end());
	}
}