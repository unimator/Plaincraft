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
#include <iostream>
#include <ctime>

namespace plaincraft_core {
	Game::Game(std::unique_ptr<plaincraft_render_engine::RenderEngine> render_engine) 
		: render_engine_(std::move(render_engine)),
		events_manager_(std::make_shared<EventsManager>()),
		physics_engine_(std::make_shared<PhysicsEngine>()),		
		scene_(Scene(events_manager_, physics_engine_)),
		input_manager_(InputManager(events_manager_, render_engine_->GetWindow()->GetInstance()))
	{
		auto window = render_engine_->GetWindow();

		glfwSetWindowUserPointer(window->GetInstance(), this);

		events_manager_->Subscribe(EventTypes::LOOP_EVENT, physics_engine_);
	}

	Game::~Game() {
	}

	void Game::Run() {
		
		WorldGenerator world_generator;
		world_generator.GenerateWorld(scene_, render_engine_);

		auto camera = render_engine_->GetCamera();
		std::shared_ptr<Player> player;
		player = std::make_shared<Player>(camera);

		auto polygon = std::make_shared<Cube>();
		const auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\player.png");
		std::shared_ptr<Texture> player_texture = std::move(render_engine_->GetTexturesFactory()->LoadFromImage(image));

		auto drawable = std::make_shared<Drawable>();
		drawable->SetModel(std::make_shared<Model>(polygon, player_texture));
		drawable->SetScale(0.5f);
		player->SetDrawable(drawable);		

		auto body = std::shared_ptr<Body>(new Body());
		auto player_position = Vector3d(2.0f, 3.0f, 2.0f);
		auto box_collider = BoxCollider(Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 0.25f, 0.25f, 0.25f);
		body->SetCollider(std::make_shared<BoxCollider>(std::move(box_collider)));
		player->SetBody(body);
		player->SetPosition(player_position);

		scene_.AddEntity(player, render_engine_);

		events_manager_->Subscribe(EventTypes::INPUT_EVENT, player);
		events_manager_->Subscribe(EventTypes::LOOP_EVENT, player);

		MainLoop();
	}

	void Game::MainLoop() {
		double cursor_position_x, cursor_position_y;
		double last_cursor_position_x_ = 1024 / 2, last_cursor_position_y_ = 768 / 8;
		auto window_instance = render_engine_->GetWindow()->GetInstance();
		double last_time, current_time = glfwGetTime();
		float delta_time;
		last_time = current_time;
		uint32_t frame_ticks = 0;
		auto camera = render_engine_->GetCamera();

		auto fps_timer = glfwGetTime();

		bool should_close = false;

		do {
			current_time = glfwGetTime();
			delta_time = static_cast<float>(current_time - last_time);
			delta_time = glm::clamp(delta_time, 0.0f, 1.0f);

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