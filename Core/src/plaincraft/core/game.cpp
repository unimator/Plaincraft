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
#include "world/world_generator.hpp"
#include "utils/conversions.hpp"
#include <reactphysics3d/reactphysics3d.h>
//#include "camera_operators/eyes/camera_operator_eyes.hpp"
#include "camera_operators/follow/camera_operator_follow.hpp"
#include <iostream>
#include <ctime>

namespace plaincraft_core {
	Game::Game(std::unique_ptr<plaincraft_render_engine::RenderEngine> render_engine) 
		: render_engine_(std::move(render_engine)),
		events_manager_(std::make_shared<EventsManager>()),	
		scene_(Scene(events_manager_)),
		input_manager_(InputManager(events_manager_, render_engine_->GetWindow()->GetInstance()))
	{
		physics_world_ = physics_common_.createPhysicsWorld();

		auto window = render_engine_->GetWindow();

		glfwSetWindowUserPointer(window->GetInstance(), this);
	}

	Game::~Game() {
	}
	std::shared_ptr<Player> player;

	void Game::Run() {
		
		WorldGenerator world_generator(physics_common_, physics_world_);
		world_generator.GenerateWorld(scene_, render_engine_);

		auto camera = render_engine_->GetCamera();
		player = std::make_shared<Player>(camera);

		auto cube_model = read_file_raw("F:/Projekty/Plaincraft/Models/cube_half.obj");
		auto mesh = std::shared_ptr<Mesh>(std::move(Mesh::LoadWavefront(cube_model.data())));

		const auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\player.png");
		std::shared_ptr<Texture> player_texture = std::move(render_engine_->GetTexturesFactory()->LoadFromImage(image));

		auto drawable = std::make_shared<Drawable>();
		drawable->SetModel(std::make_shared<Model>(mesh, player_texture));
		drawable->SetScale(0.5f);
		drawable->SetColor(Vector3d(1.0f, 0.0f, 0.0f));
		player->SetDrawable(drawable);

		auto player_position = Vector3d(0.5f, 2.0f, 0.0f);

		auto orientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
		auto rigid_body = physics_world_->createRigidBody(transform);
		auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5f, 0.5f, 0.5f) / 2.0f);
		rigid_body->addCollider(cube_shape, transform);
		//rigid_body->setTransform(transform);
		player->SetRigidBody(rigid_body);
		
		player->SetPosition(player_position);

		scene_.AddEntity(player, render_engine_);

		events_manager_->Subscribe(EventTypes::INPUT_EVENT, player);
		events_manager_->Subscribe(EventTypes::LOOP_EVENT, player);

		camera_operator_ = std::make_unique<CameraOperatorFollow>(render_engine_->GetCamera(), player);

		MainLoop();
	}

	void Game::MainLoop() {
		double cursor_position_x, cursor_position_y;
		double last_cursor_position_x_ = 1024 / 2, last_cursor_position_y_ = 768 / 2;
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

			physics_world_->update(delta_time);
			
			//player->GetDrawable()->SetPosition(Vector3d(position.x, position.y, position.z));
			//player->GetDrawable()->SetRotation(Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));

			scene_.UpdateFrame();

			events_manager_->Trigger(LoopEvent(delta_time));

			render_engine_->GetCursorPosition(&cursor_position_x, &cursor_position_y);
			camera_operator_->HandleCameraMovement(cursor_position_x - last_cursor_position_x_, last_cursor_position_y_ - cursor_position_y, delta_time);

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