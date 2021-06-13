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

namespace plaincraft_core {
	using namespace plaincraft_render_engine;
	using namespace plaincraft_render_engine_opengl;

	Scene::Scene(std::shared_ptr<EventsManager> events_manager, std::shared_ptr<RenderEngine> render_engine)
		: events_manager_(std::move(events_manager)), 
		  render_engine_(std::move(render_engine)){

		auto camera = render_engine_->GetCamera();
		//camera_->position = glm::vec3(0.0f, 0.75f, 3.0f);
		camera->direction = glm::vec3(0.0f, 0.0f, -1.0f);
		camera->up = glm::vec3(0.0f, 1.0f, 0.0f);
		camera->fov = 45.0f;

		physics_engine_ = std::shared_ptr<PhysicsEngine>(new PhysicsEngine());
		events_manager_->Subscribe(EventTypes::LOOP_EVENT, physics_engine_);

		std::shared_ptr<Player> player;
		player = std::shared_ptr<Player>(new Player(camera));
		auto polygon = std::shared_ptr<Polygon>(new Cube());
		const auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\player.png");
		std::shared_ptr<Texture> player_texture = std::move(render_engine_->GetTexturesFactory()->LoadFromImage(image));
		auto drawable = std::shared_ptr<Drawable>(new Drawable());
		
		drawable->SetModel(std::make_shared<Model>(Model(polygon, player_texture)));
		drawable->SetScale(0.5f);
		player->SetDrawable(drawable);
		drawable->SetShader(render_engine_->GetShadersRepository()->GetShader("default"));

		auto body = std::shared_ptr<Body>(new Body());
		auto player_position = Vector3d(2.0f, 3.0f, 2.0f);
		auto box_collider = BoxCollider(Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 0.25f, 0.25f, 0.25f);
		body->SetCollider(std::make_shared<BoxCollider>(std::move(box_collider)));
		player->SetBody(body);
		player->SetPosition(player_position);
		AddEntity(player);

		events_manager_->Subscribe(EventTypes::INPUT_EVENT, player);
		events_manager_->Subscribe(EventTypes::LOOP_EVENT, player);

	}

	void Scene::AddEntity(std::shared_ptr<Entity> entity) {
		entities_list_.push_back(entity);
		render_engine_->AddDrawable(entity->GetDrawable());
		physics_engine_->AddBody(entity->GetBody());
	}
}