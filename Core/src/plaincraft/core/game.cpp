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

#include "camera_operators/eyes/camera_operator_eyes.hpp"
#include "camera_operators/follow/camera_operator_follow.hpp"
#include "entities/game_object.hpp"
#include "entities/map/map.hpp"
#include "game.hpp"
#include "initialization/scene_builder.hpp"
#include "physics/physics_object.hpp"
#include "utils/conversions.hpp"
#include "world/world_generator.hpp"
#include <ctime>
#include <functional>
#include <exception>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <typeinfo>

namespace plaincraft_core
{
	Game::Game(std::shared_ptr<plaincraft_render_engine::RenderEngine> render_engine)
		: render_engine_(render_engine),
		  assets_manager_(render_engine_),
		  fps_counter_(GetLoopEventsHandler()),
		  input_stack_(render_engine)
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
		return *scene_;
	}

	void Game::Initialize()
	{
		SceneBuilder scene_builder(render_engine_, assets_manager_);
		scene_ = scene_builder.CreateScene();

		auto player = scene_->FindGameObjectByName("player");

		// camera_operator_ = std::make_shared<CameraOperatorFollow>(render_engine_->GetCamera(), player);
		camera_operator_ = std::make_shared<CameraOperatorEyes>(render_engine_->GetCamera(), player);

		auto map = std::dynamic_pointer_cast<Map>(scene_->FindGameObjectByName("map"));

		auto seed = global_state_.GetSeed();
		// auto chunk_builder = std::make_unique<SimpleChunkBuilder>(scene_);
		auto chunk_builder = std::make_unique<ChunkBuilder>(scene_, seed);
		auto world_optimizer = std::make_unique<WorldOptimizer>(map, assets_manager_, *(render_engine_->GetModelsFactory()));
		world_updater_ = std::make_unique<WorldGenerator>(std::move(world_optimizer), std::move(chunk_builder), scene_, map, player);

		auto &fonts_factory = render_engine_->GetFontsFactory();
		auto default_fonts = fonts_factory->LoadStandardFonts();

		for (auto &default_font : default_fonts)
		{
			fonts_cache_.Store(default_font.first, default_font.second);
		}

		loop_events_handler_.loop_event_trigger.AddSubscription(world_updater_.get(), &WorldGenerator::OnLoopFrameTick);
		loop_events_handler_.loop_event_trigger.AddSubscription(camera_operator_.get(), &CameraOperator::OnLoopFrameTick);

		GetWindowEventsHandler().key_pressed_event_trigger.AddSubscription(&input_stack_, &InputStack::SingleClickHandler);
		loop_events_handler_.mouse_movement_trigger.AddSubscription(&input_stack_, &InputStack::MouseMovement);

		player_input_controller_ = std::make_unique<EntityInputController>(player, render_engine_->GetCamera());
		loop_events_handler_.loop_event_trigger.AddSubscription(player_input_controller_.get(), &EntityInputController::OnLoopTick);
		input_stack_.Push(std::ref(player_input_controller_->GetInputTarget()));

		in_game_menu_controller_ = std::make_unique<InGameMenuController>(render_engine_, global_state_, fonts_cache_, input_stack_);
		input_stack_.Push(std::ref(in_game_menu_controller_->GetInputTarget()));

		camera_controller_ = std::make_unique<CameraController>(camera_operator_);
		input_stack_.Push(std::ref(camera_controller_->GetInputTarget()));
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

		auto player = scene_->FindGameObjectByName("player");
		auto map = std::dynamic_pointer_cast<Map>(scene_->FindGameObjectByName("map"));

		do
		{
			current_time = glfwGetTime();
			delta_time = static_cast<float>(current_time - last_time);
			delta_time = glm::clamp(delta_time, 0.0f, 1.0f);
			accumulator += delta_time;

			last_time = current_time;

			render_engine_->GetCursorPosition(&cursor_position_x, &cursor_position_y);

			Profiler::ProfileInfo info;
			info.name = "Physics";
			Profiler::Start(info);
			while (accumulator >= physics_time_step_)
			{
				scene_->GetPhysicsEngine().Step(physics_time_step_);
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

			MEASURE("mouse movement", {
				loop_events_handler_.mouse_movement_trigger.Trigger(cursor_position_x - last_cursor_position_x_, last_cursor_position_y_ - cursor_position_y, delta_time);
			});

			//camera_operator_->HandleCameraMovement(cursor_position_x - last_cursor_position_x_, last_cursor_position_y_ - cursor_position_y, delta_time);

			plaincraft_render_engine::FrameConfig frame_config{
				global_state_.GetDebugInfoVisibility()};
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

		} while (global_state_.GetIsRunning() && glfwWindowShouldClose(window_instance) == 0);
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