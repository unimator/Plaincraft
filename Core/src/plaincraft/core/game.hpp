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

#include "common.hpp"
#include "scene/scene.hpp"
#include "camera_operators/camera_operator.hpp"
#include "events/loop_events_handler.hpp"
#include "world/world_generator.hpp"
#include "world/world_updater.hpp"
#include "physics_optimization/active_objects_optimizer.hpp"
#include <plaincraft_render_engine.hpp>

namespace plaincraft_core {
	class Game {
	private:
		std::shared_ptr<plaincraft_render_engine::RenderEngine> render_engine_;
		std::unique_ptr<CameraOperator> camera_operator_;
		
		Cache<Model> models_cache_;
		Cache<Texture> textures_cache_;
		
		LoopEventsHandler loop_events_handler_;

		rp3d::PhysicsCommon physics_common_;
		std::shared_ptr<rp3d::PhysicsWorld> physics_world_;
		float physics_time_step_ = 1.0f / 60.0f;

		Scene scene_;
		
		std::unique_ptr<WorldUpdater> world_updater_;
		std::unique_ptr<ActiveObjectsOptimizer> active_objects_optimizer_;

	public:
		Game(std::shared_ptr<plaincraft_render_engine::RenderEngine> renderEngine);
		~Game();

		void Run();

		auto GetScene() -> Scene& {return scene_;}
		WindowEventsHandler& GetWindowEventsHandler();
		LoopEventsHandler& GetLoopEventsHandler();
		std::shared_ptr<Camera> GetCamera();

	private:
		void MainLoop();
		void Initialize();
	};
}
#endif // PLAINCRAFT_CORE_GAME