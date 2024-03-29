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

#ifndef PLAINCRAFT_CORE_GAME
#define PLAINCRAFT_CORE_GAME

#include "assets/assets_manager.hpp"
#include "camera_operators/camera_operator.hpp"
#include "common.hpp"
#include "controllers/camera_controller.hpp"
#include "controllers/entity_input_controller.hpp"
#include "controllers/in_game_menu_controller.hpp"
#include "events/loop_events_handler.hpp"
#include "input/input_stack.hpp"
#include "physics/physics_engine.hpp"
#include "physics_optimization/active_objects_optimizer.hpp"
#include "scene/scene.hpp"
#include "state/global_state.hpp"
#include "utils/fps_counter.hpp"
#include "world/chunks/chunk_builder.hpp"
#include "world/chunks/simple_chunk_builder.hpp"
#include "world/world_generator.hpp"
#include <plaincraft_render_engine.hpp>

namespace plaincraft_core {
	class Game {
	private:
		GlobalState global_state_;
		std::shared_ptr<Menu> in_game_menu_;

		std::shared_ptr<plaincraft_render_engine::RenderEngine> render_engine_;
		std::shared_ptr<Scene> scene_;
		std::shared_ptr<CameraOperator> camera_operator_;
		
		Cache<Font> fonts_cache_;

		AssetsManager assets_manager_;
		
		LoopEventsHandler loop_events_handler_;
		FpsCounter fps_counter_;
		InputStack input_stack_;

		float physics_time_step_ = 1.0f / 60.0f;

		std::unique_ptr<EntityInputController> player_input_controller_;
		std::unique_ptr<InGameMenuController> in_game_menu_controller_;
		std::unique_ptr<CameraController> camera_controller_;
		std::unique_ptr<WorldGenerator> world_updater_;
		std::unique_ptr<ActiveObjectsOptimizer> active_objects_optimizer_;

	public:
		Game(std::shared_ptr<plaincraft_render_engine::RenderEngine> renderEngine);
		~Game();

		void Run();

		Scene& GetScene();
		GlobalState& GetGlobalState();
		WindowEventsHandler& GetWindowEventsHandler();
		LoopEventsHandler& GetLoopEventsHandler();
		std::shared_ptr<Camera> GetCamera();

	private:
		void MainLoop();
		void Initialize();
	};
}
#endif // PLAINCRAFT_CORE_GAME