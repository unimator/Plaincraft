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
#include "./input/input_manager.hpp"
#include "./events/events_manager.hpp"
#include "scene.hpp"
#include "camera_operators/camera_operator.hpp"
#include "models/models_cache.hpp"
#include <plaincraft_render_engine.hpp>

namespace plaincraft_core {
	class Game {
		friend class InputManager;

	private:
		std::unique_ptr<plaincraft_render_engine::RenderEngine> render_engine_;
		std::shared_ptr<EventsManager> events_manager_;
		std::unique_ptr<CameraOperator> camera_operator_;
		Scene scene_;
		InputManager input_manager_;
		ModelsCache models_cache_;

		rp3d::PhysicsCommon physics_common_;
		rp3d::PhysicsWorld* physics_world_;
		float physics_time_step_ = 1.0f / 60.0f;

	public:
		Game(std::unique_ptr<plaincraft_render_engine::RenderEngine> renderEngine);
		~Game();

		void Run();

		auto GetScene() -> Scene& {return scene_;}
		auto GetEventsManager() -> std::shared_ptr<EventsManager> { return events_manager_; }
		std::shared_ptr<Camera> GetCamera();

	private:
		void MainLoop();
		void Initialize();
	};
}
#endif // PLAINCRAFT_CORE_GAME