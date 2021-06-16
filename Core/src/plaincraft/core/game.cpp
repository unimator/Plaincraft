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

#include "game.hpp"
#include "entities/entity.hpp"
#include "entities/player.hpp"
#include "events/types/loop_event.hpp"
#include "physics/collider/box_collider.hpp"
#include "world/world_generator.hpp"
#include <plaincraft_render_engine.hpp>
#include <plaincraft_render_engine_vulkan.hpp>
#include <plaincraft_render_engine_opengl.hpp>
#include <plaincraft_render_engine.hpp>
#include <iostream>
#include <ctime>

namespace plaincraft_core {
	Game::Game() {
		const auto camera = std::shared_ptr<plaincraft_render_engine::Camera>(new plaincraft_render_engine::Camera());
		//render_engine_ = std::shared_ptr<plaincraft_render_engine_opengl::OpenGLRenderEngine>(new plaincraft_render_engine_opengl::OpenGLRenderEngine(1024, 768));
		render_engine_ = std::shared_ptr<plaincraft_render_engine_vulkan::VulkanRenderEngine>(new plaincraft_render_engine_vulkan::VulkanRenderEngine(1024, 768, true));
		render_engine_->GetShadersRepository()->RegisterShader("default", render_engine_->CreateDefaultShader());
		glfwSetWindowUserPointer(render_engine_->GetWindow(), this);

		events_manager_ = std::shared_ptr<EventsManager>(new EventsManager());
		input_manager_ = std::shared_ptr<InputManager>(new InputManager(events_manager_, render_engine_->GetWindow()));
		scene_ = std::shared_ptr<Scene>(new Scene(events_manager_, render_engine_));
	}

	Game::~Game() {
	}

	void Game::Run() {
		
		WorldGenerator world_generator;
		world_generator.GenerateWorld(scene_, render_engine_);
		MainLoop();
	}

	void Game::MainLoop() {
		double cursor_position_x, cursor_position_y;
		double last_cursor_position_x_ = 1024 / 2, last_cursor_position_y_ = 768 / 8;
		auto window_instance = render_engine_->GetWindow();
		double last_time, current_time = glfwGetTime();
		float delta_time;
		last_time = current_time;
		uint32_t frame_ticks = 0, logic_ticks = 0;
		auto camera = render_engine_->GetCamera();

		auto fps_timer = glfwGetTime();

		bool should_close = false;

		do {
			current_time = glfwGetTime();
			delta_time = static_cast<float>(current_time - last_time);
			last_time = current_time;

			events_manager_->Trigger(LoopEvent(delta_time));

			render_engine_->GetCursorPosition(&cursor_position_x, &cursor_position_y);
			camera->HandleCameraMovement(cursor_position_x - last_cursor_position_x_, last_cursor_position_y_ - cursor_position_y, delta_time);

			//scene_->Render();

			render_engine_->RenderFrame();

			last_cursor_position_x_ = cursor_position_x;
			last_cursor_position_y_ = cursor_position_y;

			should_close = glfwGetKey(window_instance, GLFW_KEY_ESCAPE) == GLFW_PRESS;

			++frame_ticks;

			if (current_time - fps_timer > 1.0f) {
				//printf("%u\n", frame_ticks);
				frame_ticks = 0;
				fps_timer = current_time;
			}

		} while (!should_close && glfwWindowShouldClose(window_instance) == 0);
	}
}