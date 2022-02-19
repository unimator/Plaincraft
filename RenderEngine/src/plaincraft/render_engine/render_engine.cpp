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
		textures_repository_ = std::make_shared<TexturesRepository>();

		camera_->direction = glm::vec3(0.0f, 0.0f, -1.0f);
		camera_->up = glm::vec3(0.0f, 1.0f, 0.0f);
		camera_->fov = 45.0f;
	}

	RenderEngine::~RenderEngine(){

	};

	RenderEngine::RenderEngine(RenderEngine &&other)
		: window_(std::move(other.window_))
	{
		this->camera_ = other.camera_;
		other.camera_ = nullptr;

		this->drawables_list_ = std::move(other.drawables_list_);

		this->textures_factory_ = other.textures_factory_;
		other.textures_factory_ = nullptr;

		this->textures_repository_ = other.textures_repository_;
		other.textures_repository_ = nullptr;
	}

	RenderEngine &RenderEngine::operator=(RenderEngine &&other)
	{
		this->camera_ = other.camera_;
		other.camera_ = nullptr;

		this->drawables_list_ = std::move(other.drawables_list_);

		this->textures_factory_ = other.textures_factory_;
		other.textures_factory_ = nullptr;

		this->textures_repository_ = other.textures_repository_;
		other.textures_repository_ = nullptr;

		return *this;
	}

	void RenderEngine::GetCursorPosition(double *cursor_position_x, double *cursor_position_y)
	{
		glfwGetCursorPos(window_->GetInstance(), cursor_position_x, cursor_position_y);
	}

	std::shared_ptr<TexturesFactory> RenderEngine::GetTexturesFactory()
	{
		return textures_factory_;
	}

	std::shared_ptr<TexturesRepository> RenderEngine::GetTexturesRepository()
	{
		return textures_repository_;
	}

	std::shared_ptr<ModelsFactory> RenderEngine::GetModelsFactory()
	{
		return models_factory_;
	}

	void RenderEngine::AddDrawable(std::shared_ptr<Drawable> drawable_to_add)
	{
		drawables_list_.push_back(drawable_to_add);
	}

	void RenderEngine::RemoveDrawable(std::shared_ptr<Drawable> drawable_to_remove)
	{
		drawables_list_.erase(std::remove_if(drawables_list_.begin(),
											 drawables_list_.end(),
											 [&](std::shared_ptr<Drawable> const &drawable)
											 { return drawable == drawable_to_remove; }));
	}
}