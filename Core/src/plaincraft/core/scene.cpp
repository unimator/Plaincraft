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

#include "scene.hpp"
#include "entities/player.hpp"
#include "physics/collider/box_collider.hpp"
#include "physics/physics_engine.hpp"
#include "physics/collisions/collisions_detector.hpp"
#include <plaincraft_render_engine.hpp>
#include <plaincraft_render_engine_opengl.hpp>

namespace plaincraft_core
{
	using namespace plaincraft_render_engine;
	using namespace plaincraft_render_engine_opengl;

	Scene::Scene(std::shared_ptr<EventsManager> events_manager, std::shared_ptr<PhysicsEngine> physics_engine)
		: events_manager_(std::move(events_manager)),
		physics_engine_(std::move(physics_engine))
	{
	}

	void Scene::AddEntity(std::shared_ptr<Entity> entity, std::unique_ptr<RenderEngine>& render_engine)
	{
		entities_list_.push_back(entity);
		render_engine->AddDrawable(entity->GetDrawable());
		physics_engine_->AddBody(entity->GetBody());
	}
}