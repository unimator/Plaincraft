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

#ifndef PLAINCRAFT_CORE_SCENE
#define PLAINCRAFT_CORE_SCENE

// namespace plaincraft_core 
// {
// 	class Scene;
// }

#include "../common.hpp"
#include "../entities/game_object.hpp"
#include "./events/scene_events_handler.hpp"
#include "../physics/physics_engine.hpp"
#include "../utils/conversions.hpp"
#include <plaincraft_render_engine.hpp>
#include <memory>
#include <mutex>

namespace std 
{
	template<>
	struct less<std::reference_wrapper<plaincraft_core::GameObject>> {
		bool operator()(std::reference_wrapper<plaincraft_core::GameObject> first_entity, 
			std::reference_wrapper<plaincraft_core::GameObject> second_entity) const;
	};
}

namespace plaincraft_core
{
	using namespace plaincraft_render_engine;

	class Scene
	{
	private:
		mutable std::mutex scene_access_;
		std::shared_ptr<RenderEngine> render_engine_;
		mutable std::list<std::shared_ptr<GameObject>> game_objects_list_;

		SceneEventsHandler scene_events_handler_;

		PhysicsEngine physics_engine_;
		
	public:
		Scene(std::shared_ptr<RenderEngine> render_engine, PhysicsEngine::PhysicsSettings physics_settings, std::shared_ptr<Map> map);
		~Scene();

		void AddGameObject(std::shared_ptr<GameObject> game_object_to_add);
		void RemoveGameObject(std::shared_ptr<GameObject> game_object_to_remove);
		void RemoveGameObjects(std::vector<std::shared_ptr<GameObject>> &game_objects_to_remove);
		
		std::shared_ptr<GameObject> FindGameObjectByName(const std::string& name) const;

		SceneEventsHandler& GetSceneEventsHandler();
		PhysicsEngine& GetPhysicsEngine();
	};
}

#endif // PLAINCRAFT_CORE_SCENE