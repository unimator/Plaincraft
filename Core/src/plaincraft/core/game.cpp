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
#include "physics/physics_object.hpp"
#include <iostream>
#include <ctime>
#include <string>
#include <random>
#include <exception>
#include <typeinfo>
#include <stdexcept>

namespace plaincraft_core
{
	Game::Game(std::shared_ptr<plaincraft_render_engine::RenderEngine> render_engine)
		: render_engine_(std::move(render_engine)),
		  scene_(Scene(render_engine_)),
		  fps_counter_(GetLoopEventsHandler())
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist;
		// global_state_.SetSeed((static_cast<uint64_t>(dist(rng)) << 32) + dist(rng));
		global_state_.SetSeed(5834094764593785);
		Initialize();
	}

	Game::~Game()
	{
	}

	std::shared_ptr<Camera> Game::GetCamera()
	{
		return render_engine_->GetCamera();
	}

	GlobalState &Game::GetGlobalState()
	{
		return global_state_;
	}

	Scene &Game::GetScene()
	{
		return scene_;
	}

	void Game::Initialize()
	{
		std::shared_ptr<GameObject> player;

		auto sphere_model_obj = read_file_raw("F:/Projekty/Plaincraft/Assets/Models/sphere_half.obj");
		std::shared_ptr<Mesh> sphere_mesh = std::move(Mesh::LoadWavefront(sphere_model_obj.data()));
		auto sphere_model = render_engine_->GetModelsFactory()->CreateModel(sphere_mesh);
		models_cache_.Store("sphere_half", std::move(sphere_model));

		auto capsule_model_obj = read_file_raw("F:/Projekty/Plaincraft/Assets/Models/capsule_half.obj");
		std::shared_ptr<Mesh> capsule_mesh = std::move(Mesh::LoadWavefront(capsule_model_obj.data()));
		auto capsule_model = render_engine_->GetModelsFactory()->CreateModel(capsule_mesh);
		models_cache_.Store("capsule_half", std::move(capsule_model));

		auto player_cuboid_obj = read_file_raw("F:/Projekty/Plaincraft/Assets/Models/player_cuboid.obj");
		std::shared_ptr<Mesh> player_cuboid_mesh = std::move(Mesh::LoadWavefront(player_cuboid_obj.data()));
		auto player_cuboid_model = render_engine_->GetModelsFactory()->CreateModel(player_cuboid_mesh);
		models_cache_.Store("player_cuboid", std::move(player_cuboid_model));

		auto cube_obj = read_file_raw("F:/Projekty/Plaincraft/Assets/Models/cube.obj");
		std::shared_ptr<Mesh> cube_mesh = std::move(Mesh::LoadWavefront(cube_obj.data()));
		auto cube_model = render_engine_->GetModelsFactory()->CreateModel(cube_mesh);
		models_cache_.Store("cube", std::move(cube_model));

		auto minecraft_texture_image = load_bmp_image_from_file("F:/Projekty/Plaincraft/Assets/Textures/minecraft-textures.png");
		auto minecraft_texture = render_engine_->GetTexturesFactory()->LoadFromImage(minecraft_texture_image);
		textures_cache_.Store("minecraft-texture", std::move(minecraft_texture));

		player = std::make_shared<GameObject>();
		player->SetName("player");

		auto player_model = models_cache_.Fetch("player_cuboid");
		auto drawable = std::make_shared<Drawable>();
		drawable->SetModel(player_model);
		drawable->SetTexture(textures_cache_.Fetch("minecraft-texture"));
		drawable->SetScale(1.0f);
		drawable->SetColor(Vector3d(1.0f, 1.0f, 1.0f));
		player->SetDrawable(drawable);

		auto player_physics_object = std::make_shared<PhysicsObject>();
		auto player_position = Vector3d(8.0f, 55.0f, 16.0f);
		auto player_size = Vector3d(0.8f, 1.8f, 0.8f);
		player_physics_object->position = player_position;
		player_physics_object->size = player_size;
		player_physics_object->friction = 10.0f;
		player_physics_object->type = PhysicsObject::ObjectType::Dynamic;
		player->SetPhysicsObject(player_physics_object);

		scene_.AddGameObject(player);

		camera_operator_ = std::make_unique<CameraOperatorFollow>(render_engine_->GetCamera(), player);
		// camera_operator_ = std::make_unique<CameraOperatorEyes>(render_engine_->GetCamera(), player);

		auto map = std::make_shared<Map>();
		map->SetName("map");
		scene_.AddGameObject(map);
		PhysicsEngine::PhysicsSettings physics_settings{Vector3d(0.0f, -9.81, 0.0f)};
		physics_engine_ = std::make_unique<PhysicsEngine>(physics_settings, map);
		physics_engine_->AddObject(player_physics_object);

		auto seed = global_state_.GetSeed();
		// auto chunk_builder = std::make_unique<SimpleChunkBuilder>(scene_);
		auto chunk_builder = std::make_unique<ChunkBuilder>(scene_, seed);
		auto world_optimizer = std::make_unique<WorldOptimizer>(map, models_cache_, textures_cache_, render_engine_->GetModelsFactory());
		world_updater_ = std::make_unique<WorldGenerator>(std::move(world_optimizer), std::move(chunk_builder), scene_, map, player);

		loop_events_handler_.loop_event_trigger.AddSubscription(&scene_, &Scene::UpdateFrame);
		loop_events_handler_.loop_event_trigger.AddSubscription(world_updater_.get(), &WorldGenerator::OnLoopFrameTick);
	}

	void Game::Run()
	{
		try
		{
			MainLoop();
		}
		catch (const std::runtime_error &re)
		{
			// speciffic handling for runtime_error
			std::cerr << "Runtime error: " << re.what() << std::endl;
		}
		catch (const std::exception &ex)
		{
			// speciffic handling for all exceptions extending std::exception, except
			// std::runtime_error which is handled explicitly
			std::cerr << "Error occurred: " << ex.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "error" << std::endl;
		}
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

		auto player = scene_.FindGameObjectByName("player");
		auto map = std::dynamic_pointer_cast<Map>(scene_.FindGameObjectByName("map"));

		do
		{
			current_time = glfwGetTime();
			delta_time = static_cast<float>(current_time - last_time);
			delta_time = glm::clamp(delta_time, 0.0f, 1.0f);
			accumulator += delta_time;

			last_time = current_time;

			Profiler::ProfileInfo info;
			info.name = "Physics";
			Profiler::Start(info);
			while (accumulator >= physics_time_step_)
			{
				auto position = player->GetPhysicsObject()->position;
				auto pos_x = static_cast<int32_t>(position.x);
				auto pos_z = static_cast<int32_t>(position.z);
				auto chunk_size = static_cast<int32_t>(Chunk::chunk_size);
				auto x = pos_x / chunk_size - (pos_x % chunk_size < 0 ? 1 : 0);
				auto z = pos_z / chunk_size - (pos_z % chunk_size < 0 ? 1 : 0);
				// auto z = static_cast<int32_t>(position.z) / static_cast<int32_t>(Chunk::chunk_size);

				auto grid = map->GetGrid();

				for (auto i = 0; i < Map::render_diameter; ++i)
				{
					for (auto j = 0; j < Map::render_diameter; ++j)
					{
						grid[i][j]->SetColor(Vector3d(1.0f, 1.0f, 1.0f));
					}
				}

				auto chunk_x = x - grid[0][0]->GetPositionX();
				auto chunk_z = z - grid[0][0]->GetPositionZ();

				// if(chunk_x < 0 || chunk_x >= Map::render_diameter || chunk_z < 0 || chunk_z >= Map::render_diameter)
				// {
				// 	break;
				// }

				// One chunk of "safety"
				if (chunk_x < 1 || chunk_x >= Map::render_diameter - 1 || chunk_z < 1 || chunk_z >= Map::render_diameter - 1)
				{
					break;
				}

				auto &chunk = grid[chunk_x][chunk_z];
				chunk->SetColor(Vector3d(1.0f, 0.0f, 0.0f));

				const char *format = "(%d, %d)";
				std::vector<char> buffer(1024);
				std::snprintf(&buffer[0], buffer.size(), format, chunk_x + grid[0][0]->GetPositionX(), chunk_z + grid[0][0]->GetPositionZ());
				LOGVALUE("Active chunk", std::string(buffer.begin(), buffer.end()));

				physics_engine_->Step(physics_time_step_);
				accumulator -= physics_time_step_;
			}
			Profiler::End(info);

			player->GetDrawable()->SetPosition(player->GetPhysicsObject()->position);

			MEASURE("update scene objects", {
												// scene_.UpdateFrame();
											});

			MEASURE("loop events", {
				loop_events_handler_.loop_event_trigger.Trigger(delta_time);
			});

			render_engine_->GetCursorPosition(&cursor_position_x, &cursor_position_y);
			camera_operator_->HandleCameraMovement(cursor_position_x - last_cursor_position_x_, last_cursor_position_y_ - cursor_position_y, delta_time);

			plaincraft_render_engine::FrameConfig frame_config{
				global_state_.GetDebugInfoVisibility()};
			MEASURE("graphics render",
					{
						// render_engine_->RenderFrame(frame_config);
						scene_.RenderFrame(frame_config);
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

			if (player != nullptr)
			{
				auto player_position = player->GetPhysicsObject()->position;
				const char *format = "(%f, %f, %f)";
				std::vector<char> buffer(1024);
				std::snprintf(&buffer[0], buffer.size(), format, player_position.x, player_position.y, player_position.z);
				LOGVALUE("player position", std::string(buffer.begin(), buffer.end()));
				auto player_velocity = player->GetPhysicsObject()->velocity;
				std::snprintf(&buffer[0], buffer.size(), format, player_velocity.x, player_velocity.y, player_velocity.z);
				LOGVALUE("player velocity", std::string(buffer.begin(), buffer.end()));
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