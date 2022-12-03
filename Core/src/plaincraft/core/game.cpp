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
#include "entities/game_object.hpp"
#include "world/world_generator.hpp"
#include "entities/map/map.hpp"
#include "utils/conversions.hpp"
#include "camera_operators/eyes/camera_operator_eyes.hpp"
#include "camera_operators/follow/camera_operator_follow.hpp"
#include <iostream>
#include <ctime>
#include <string>
#include <random>

namespace plaincraft_core
{
	Game::Game(std::shared_ptr<plaincraft_render_engine::RenderEngine> render_engine)
		: render_engine_(std::move(render_engine)),
		  scene_(Scene(render_engine_)),
		  fps_counter_(GetLoopEventsHandler())
	{
		Initialize();
		std::random_device dev;
    	std::mt19937 rng(dev());
    	std::uniform_int_distribution<std::mt19937::result_type> dist;
		global_state_.SetSeed((static_cast<uint64_t>(dist(rng)) << 32) + dist(rng));
	}

	Game::~Game()
	{
	}

	std::shared_ptr<Camera> Game::GetCamera()
	{
		return render_engine_->GetCamera();
	}

	GlobalState& Game::GetGlobalState() 
	{
		return global_state_;
	}

	Scene& Game::GetScene() 
	{
		return scene_;
	}

	void Game::Initialize()
	{
		std::shared_ptr<GameObject> player;

		// physics_world_ = std::shared_ptr<rp3d::PhysicsWorld>();
		auto physics_world_settings = rp3d::PhysicsWorld::WorldSettings();
		physics_world_ = std::shared_ptr<rp3d::PhysicsWorld>(physics_common_.createPhysicsWorld(physics_world_settings), rp3d::PhysicsWorldDeleter(physics_common_));
		physics_world_->setIsDebugRenderingEnabled(false);
		// physics_world_->setGravity(rp3d::Vector3(0, 0, 0));
		// physics_world_ = physics_common_.createPhysicsWorld(physics_world_settings);

		// auto logger = physics_common_.createDefaultLogger();
		// uint32_t log_level = static_cast<uint32_t>(static_cast<uint32_t>(rp3d::Logger::Level::Warning)
		// 	| static_cast<uint32_t>(rp3d::Logger::Level::Information)
		// 	| static_cast<uint32_t>(rp3d::Logger::Level::Error));
		// logger->addStreamDestination(std::cout, log_level, rp3d::DefaultLogger::Format::Text);
		// physics_common_.setLogger(logger);

		// auto cube_model_obj = read_file_raw("F:/Projekty/Plaincraft/Assets/Models/cube_half.obj");
		// std::shared_ptr<Mesh> cube_mesh = std::move(Mesh::LoadWavefront(cube_model_obj.data()));
		// auto cube_model = render_engine_->GetModelsFactory()->CreateModel(cube_mesh);
		// models_cache_.Store("cube_half", std::move(cube_model));

		auto sphere_model_obj = read_file_raw("F:/Projekty/Plaincraft/Assets/Models/sphere_half.obj");
		std::shared_ptr<Mesh> sphere_mesh = std::move(Mesh::LoadWavefront(sphere_model_obj.data()));
		auto sphere_model = render_engine_->GetModelsFactory()->CreateModel(sphere_mesh);
		models_cache_.Store("sphere_half", std::move(sphere_model));

		auto capsule_model_obj = read_file_raw("F:/Projekty/Plaincraft/Assets/Models/capsule_half.obj");
		std::shared_ptr<Mesh> capsule_mesh = std::move(Mesh::LoadWavefront(capsule_model_obj.data()));
		auto capsule_model = render_engine_->GetModelsFactory()->CreateModel(capsule_mesh);
		models_cache_.Store("capsule_half", std::move(capsule_model));

		auto minecraft_texture_image = load_bmp_image_from_file("F:/Projekty/Plaincraft/Assets/Textures/minecraft-textures.png");
		auto minecraft_texture = render_engine_->GetTexturesFactory()->LoadFromImage(minecraft_texture_image);
		textures_cache_.Store("minecraft-texture", std::move(minecraft_texture));

		player = std::make_shared<GameObject>();
		player->SetName("player");

		auto player_model = models_cache_.Fetch("capsule_half");
		auto drawable = std::make_shared<Drawable>();
		drawable->SetModel(player_model);
		drawable->SetTexture(textures_cache_.Fetch("minecraft-texture"));
		drawable->SetScale(0.75f);
		drawable->SetColor(Vector3d(1.0f, 0.0f, 0.0f));
		player->SetDrawable(drawable);

		auto player_position = Vector3d(0.25f, 100.0f, 0.25f);

		auto orientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
		auto rigid_body = physics_world_->createRigidBody(transform);

		rigid_body->setAngularLockAxisFactor(rp3d::Vector3(0.0f, 1.0f, 0.0f));
		// rigid_body->setLinearLockAxisFactor(rp3d::Vector3(0.0f, 1.0f, 0.0f));
		auto capsule_shape = physics_common_.createCapsuleShape(0.5, 0.75);
		// auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5f, 0.5f, 0.5f) / 2.0f);
		auto collider = rigid_body->addCollider(capsule_shape, transform);
		collider->getMaterial().setFrictionCoefficient(1.0f);

		rigid_body->setTransform(rp3d::Transform(FromGlm(player_position), orientation));
		rigid_body->setIsFrictionFreezed(true);
		rigid_body->setIsBouncingFreezed(true);
		player->SetRigidBody(rigid_body);

		scene_.AddGameObject(player);

		//camera_operator_ = std::make_unique<CameraOperatorFollow>(render_engine_->GetCamera(), player);
		camera_operator_ = std::make_unique<CameraOperatorEyes>(render_engine_->GetCamera(), player);

		auto map = std::make_shared<Map>();
		scene_.AddGameObject(map);

		auto seed = global_state_.GetSeed();
		auto chunk_builder = std::make_unique<ChunkBuilder>(physics_common_, physics_world_, render_engine_, scene_, models_cache_, textures_cache_, seed);
		auto world_optimizer = std::make_unique<WorldOptimizer>(map, models_cache_, textures_cache_, render_engine_->GetModelsFactory());
		world_updater_ = std::make_unique<WorldGenerator>(std::move(world_optimizer), std::move(chunk_builder), scene_, map, player);

		active_objects_optimizer_ = std::make_unique<ActiveObjectsOptimizer>(
			scene_.GetGameObjectsList(),
			scene_.GetStaticGameObjectsList(),
			scene_.GetDynamicGameOjectsList());

		loop_events_handler_.loop_event_trigger.AddSubscription(&scene_, &Scene::UpdateFrame);
		loop_events_handler_.loop_event_trigger.AddSubscription(world_updater_.get(), &WorldGenerator::OnLoopFrameTick);
		loop_events_handler_.loop_event_trigger.AddSubscription(active_objects_optimizer_.get(), &ActiveObjectsOptimizer::OnLoopFrameTick);
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

		char buffer[0x20];
		memset(buffer, 0, 0x20);

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

			MEASURE("update scene objects", {
												// scene_.UpdateFrame();
											});

			auto player = scene_.FindGameObjectByName("player");
			if (player != nullptr)
			{
				auto player_position = player->GetRigidBody()->getTransform().getPosition();
				const char *format = "(%f, %f, %f)";
				size_t size = std::snprintf(nullptr, 0, format, player_position.x, player_position.y, player_position.z);
				std::vector<char> buffer(size + 1);
				std::snprintf(&buffer[0], buffer.size(), format, player_position.x, player_position.y, player_position.z);
				LOGVALUE("player position", std::string(buffer.begin(), buffer.end()));
			}

			MEASURE("loop events", {
				loop_events_handler_.loop_event_trigger.Trigger(delta_time);
			});

			render_engine_->GetCursorPosition(&cursor_position_x, &cursor_position_y);
			camera_operator_->HandleCameraMovement(cursor_position_x - last_cursor_position_x_, last_cursor_position_y_ - cursor_position_y, delta_time);

			plaincraft_render_engine::FrameConfig frame_config {
				global_state_.GetDebugInfoVisibility()
			};
			MEASURE("graphics render",
					{
						render_engine_->RenderFrame(frame_config);
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

			sprintf(buffer, "%u", fps_counter_.GetFramesPerSecond());
			LOGVALUE("FPS", buffer);

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