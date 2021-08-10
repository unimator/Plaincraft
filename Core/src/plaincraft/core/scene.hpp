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
#include "common.hpp"
#include "entities/entity.hpp"
#include "events/events_manager.hpp"
#include "physics/physics_engine.hpp"
#include <plaincraft_render_engine.hpp>

namespace plaincraft_core
{
	using namespace plaincraft_render_engine;

	class Scene
	{
	private:
		std::list<std::shared_ptr<Entity>> entities_list_;
		std::shared_ptr<EventsManager> events_manager_;
		std::shared_ptr<PhysicsEngine> physics_engine_;
		

	public:
		Scene(std::shared_ptr<EventsManager> events_manager, std::shared_ptr<PhysicsEngine> physics_engine);

		void AddEntity(std::shared_ptr<Entity> entity, std::unique_ptr<RenderEngine>& render_engine);
	};
}

#endif // PLAINCRAFT_CORE_SCENE