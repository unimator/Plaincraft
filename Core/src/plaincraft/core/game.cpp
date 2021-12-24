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
#include "world/world_generator.hpp"
#include "utils/conversions.hpp"
//#include "camera_operators/eyes/camera_operator_eyes.hpp"
#include "camera_operators/follow/camera_operator_follow.hpp"
#include <iostream>
#include <ctime>
#include <string>

namespace plaincraft_core
{
	Game::Game(std::unique_ptr<plaincraft_render_engine::RenderEngine> render_engine)
		: render_engine_(std::move(render_engine)),
		  scene_(Scene())
	{
		Initialize();
	}

	Game::~Game()
	{
	}

	std::shared_ptr<Camera> Game::GetCamera()
	{
		return render_engine_->GetCamera();
	}

	void Game::Initialize()
	{
		std::shared_ptr<Entity> player;

		physics_world_ = physics_common_.createPhysicsWorld();

		// auto logger = physics_common_.createDefaultLogger();
		// uint32_t log_level = static_cast<uint32_t>(static_cast<uint32_t>(rp3d::Logger::Level::Warning)
		// 	| static_cast<uint32_t>(rp3d::Logger::Level::Information)
		// 	| static_cast<uint32_t>(rp3d::Logger::Level::Error));
		// logger->addStreamDestination(std::cout, log_level, rp3d::DefaultLogger::Format::Text);
		// physics_common_.setLogger(logger);

		auto cube_model_obj = read_file_raw("F:/Projekty/Plaincraft/Models/cube_half.obj");
		auto cube_mesh = std::shared_ptr<Mesh>(std::move(Mesh::LoadWavefront(cube_model_obj.data())));
		auto cube_model = render_engine_->GetModelsFactory()->CreateModel(cube_mesh);
		models_cache_.Store("cube_half", std::move(cube_model));

		auto sphere_model_obj = read_file_raw("F:/Projekty/Plaincraft/Models/sphere_half.obj");
		auto sphere_mesh = std::shared_ptr<Mesh>(std::move(Mesh::LoadWavefront(sphere_model_obj.data())));
		auto sphere_model = render_engine_->GetModelsFactory()->CreateModel(sphere_mesh);
		models_cache_.Store("sphere_half", std::move(sphere_model));

		auto capsule_model_obj = read_file_raw("F:/Projekty/Plaincraft/Models/capsule_half.obj");
		auto capsule_mesh = std::shared_ptr<Mesh>(std::move(Mesh::LoadWavefront(capsule_model_obj.data())));
		auto capsule_model = render_engine_->GetModelsFactory()->CreateModel(capsule_mesh);
		models_cache_.Store("capsule_half", std::move(capsule_model));

		WorldGenerator world_generator(physics_common_, physics_world_);
		world_generator.GenerateWorld(scene_, render_engine_, models_cache_);

		player = std::make_shared<Entity>();
		player->SetName("player");

		const auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\player.png");
		std::shared_ptr<Texture> player_texture = std::move(render_engine_->GetTexturesFactory()->LoadFromImage(image));

		auto player_model = models_cache_.Fetch("capsule_half");
		auto drawable = std::make_shared<Drawable>();
		drawable->SetModel(player_model);
		drawable->SetScale(0.75f);
		drawable->SetColor(Vector3d(1.0f, 0.0f, 0.0f));
		player->SetDrawable(drawable);

		auto player_position = Vector3d(0.2f, 2.25f, 0.25f);

		auto orientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
		auto rigid_body = physics_world_->createRigidBody(transform);
		auto capsule_shape = physics_common_.createCapsuleShape(0.5, 0.75);
		// auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5f, 0.5f, 0.5f) / 2.0f);
		auto collider = rigid_body->addCollider(capsule_shape, transform);
		collider->getMaterial().setBounciness(0);
		collider->getMaterial().setFrictionCoefficient(2.0);

		rigid_body->setTransform(rp3d::Transform(FromGlm(player_position), orientation));
		rigid_body->setIsFrictionFreezed(true);
		rigid_body->setIsRotationFreezed(true);
		rigid_body->setIsBouncingFreezed(true);
		player->SetRigidBody(rigid_body);

		scene_.AddEntity(player, render_engine_);

		camera_operator_ = std::make_unique<CameraOperatorFollow>(render_engine_->GetCamera(), player);
	}

	void Game::Run()
	{
		MainLoop();
	}

	void Game::MainLoop()
	{
		static float accumulator = 0.0f;
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

		do
		{
			current_time = glfwGetTime();
			delta_time = static_cast<float>(current_time - last_time);
			delta_time = glm::clamp(delta_time, 0.0f, 1.0f);
			accumulator += delta_time;

			last_time = current_time;

			MEASURE("physics",
						 {
							 while (accumulator >= physics_time_step_)
							 {
								 physics_world_->update(physics_time_step_);
								 accumulator -= physics_time_step_;
							 }
						 })

			scene_.UpdateFrame();

			loop_events_handler_.loop_event_trigger.Trigger(delta_time);

			render_engine_->GetCursorPosition(&cursor_position_x, &cursor_position_y);
			camera_operator_->HandleCameraMovement(cursor_position_x - last_cursor_position_x_, last_cursor_position_y_ - cursor_position_y, delta_time);

			MEASURE("graphics render", 
			{
				render_engine_->RenderFrame();
			})

			last_cursor_position_x_ = cursor_position_x;
			last_cursor_position_y_ = cursor_position_y;

			should_close = glfwGetKey(window_instance, GLFW_KEY_ESCAPE) == GLFW_PRESS;

			++frame_ticks;

			if (current_time - fps_timer > 1.0f)
			{
				// printf("%u\n", frame_ticks);
				frame_ticks = 0;
				fps_timer = current_time;
			}

		} while (!should_close && glfwWindowShouldClose(window_instance) == 0);
	}

	WindowEventsHandler &Game::GetWindowEventsHandler()
	{
		return render_engine_->GetWindow()->GetWindowEventsHandler();
	}

	LoopEventsHandler &Game::GetLoopEventsHandler()
	{
		return loop_events_handler_;
	}
}