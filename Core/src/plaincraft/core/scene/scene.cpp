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
#include <plaincraft_render_engine.hpp>
#include <algorithm>
#include <iostream>

namespace std
{
	bool less<std::reference_wrapper<plaincraft_core::GameObject>>::operator()(
		std::reference_wrapper<plaincraft_core::GameObject> first_entity,
		std::reference_wrapper<plaincraft_core::GameObject> second_entity) const
	{
		return first_entity.get().GetUniqueId() < second_entity.get().GetUniqueId();
	}
}

namespace plaincraft_core
{
	using namespace plaincraft_render_engine;

	Scene::Scene(std::shared_ptr<RenderEngine> render_engine)
		: render_engine_(render_engine)
	{
	}

	Scene::~Scene()
	{
		game_objects_list_.clear();
	}

	void Scene::AddGameObject(std::shared_ptr<GameObject> game_object_to_add)
	{
		game_objects_list_.push_back(game_object_to_add);

		if (game_object_to_add->GetDrawable() != nullptr)
		{
			render_engine_->AddDrawable(game_object_to_add->GetDrawable());
		}

		auto physics_object = game_object_to_add->GetPhysicsObject();
		if(physics_object == nullptr)
		{
			return;
		}

		auto object_type = physics_object->type;
		if (object_type == PhysicsObject::ObjectType::Dynamic)
		{
			dynamic_game_objects_list_.push_back(game_object_to_add);
		}
		else if (object_type == PhysicsObject::ObjectType::Static)
		{
			static_game_objects_list_.push_back(game_object_to_add);
		}

		// auto rigid_body = game_object_to_add->GetRigidBody();
		// if (rigid_body != nullptr && object_type == GameObject::ObjectType::Dynamic)
		// {
		// 	const rp3d::Transform &transform = game_object_to_add->GetRigidBody()->getTransform();
		// 	previous_transforms_.insert({game_object_to_add, transform});
		// }

	}

	void Scene::RemoveGameObject(std::shared_ptr<GameObject> game_object_to_remove)
	{
		auto remove_predicate = [&](std::shared_ptr<GameObject> const &game_object)
		{ return game_object == game_object_to_remove; };

		game_objects_list_.remove_if(remove_predicate);

		if (game_object_to_remove->GetDrawable() != nullptr)
		{
			render_engine_->RemoveDrawable(game_object_to_remove->GetDrawable());
		}

		auto physics_object = game_object_to_remove->GetPhysicsObject();
		if(physics_object == nullptr)
		{
			return;
		}

		auto object_type = physics_object->type;
		if (object_type == PhysicsObject::ObjectType::Dynamic)
		{
			dynamic_game_objects_list_.remove_if(remove_predicate);
		}
		else if (object_type == PhysicsObject::ObjectType::Static)
		{
			static_game_objects_list_.remove_if(remove_predicate);
		}

		// auto rigid_body = game_object_to_remove->GetRigidBody();
		// if (rigid_body != nullptr && object_type == GameObject::ObjectType::Dynamic)
		// {
		// 	previous_transforms_.erase(previous_transforms_.find(game_object_to_remove));
		// }

	}

	void Scene::RemoveGameObjects(std::vector<std::shared_ptr<GameObject>> &game_objects_to_remove)
	{
		std::unordered_map<std::shared_ptr<GameObject>, bool> game_objects_to_remove_presence_map;
		std::transform(game_objects_to_remove.begin(),
					   game_objects_to_remove.end(),
					   std::inserter(game_objects_to_remove_presence_map, game_objects_to_remove_presence_map.begin()),
					   [](const std::shared_ptr<GameObject> &game_object_to_remove)
					   { return std::make_pair(game_object_to_remove, true); });
		auto remove_predicate = [&](std::shared_ptr<GameObject> const &game_object)
		{ return game_objects_to_remove_presence_map.contains(game_object); };

		game_objects_list_.remove_if(remove_predicate);
		dynamic_game_objects_list_.remove_if(remove_predicate);
		static_game_objects_list_.remove_if(remove_predicate);

		for (auto &game_object_to_remove : game_objects_to_remove)
		{
			// auto rigid_body = game_object_to_remove->GetRigidBody();
			// if (rigid_body != nullptr && object_type == GameObject::ObjectType::Dynamic)
			// {
			// 	previous_transforms_.erase(previous_transforms_.find(game_object_to_remove));
			// }

			if (game_object_to_remove->GetDrawable() != nullptr)
			{
				render_engine_->RemoveDrawable(game_object_to_remove->GetDrawable());
			}
		}
	}

	std::shared_ptr<GameObject> Scene::FindGameObjectByName(const std::string &name) const
	{
		auto predicate = [&](const std::shared_ptr<const GameObject> game_object)
		{ return game_object->GetName() == name; };
		auto result = std::find_if(begin(game_objects_list_), end(game_objects_list_), predicate);
		if (result != end(game_objects_list_))
		{
			return *result;
		}
		return nullptr;
	}

	std::list<std::shared_ptr<GameObject>> &Scene::GetGameObjectsList() const
	{
		return game_objects_list_;
	}

	std::list<std::shared_ptr<GameObject>> &Scene::GetStaticGameObjectsList() const
	{
		return static_game_objects_list_;
	}

	std::list<std::shared_ptr<GameObject>> &Scene::GetDynamicGameOjectsList() const
	{
		return dynamic_game_objects_list_;
	}

	void Scene::UpdateFrame(float interpolation_factor)
	{
		for (auto &game_object : dynamic_game_objects_list_)
		{
			// const auto rb = game_object->GetRigidBody();

			// if (rb == nullptr)
			// {
			// 	continue;
			// }

			// auto transform = rb->getTransform();
			// auto previous_transform = previous_transforms_[game_object];
			// auto interpolated_transform = rp3d::Transform::interpolateTransforms(previous_transform, transform, interpolation_factor);

			// auto position = FromRP3D(interpolated_transform.getPosition());
			// auto rotation = FromRP3D(interpolated_transform.getOrientation());

			// auto drawable = game_object->GetDrawable();

			// if (drawable == nullptr)
			// {
			// 	continue;
			// }

			// if (drawable->GetPosition() != position)
			// {
			// 	game_object->GetDrawable()->SetPosition(Vector3d(position.x, position.y, position.z));
			// }

			// if (drawable->GetRotation() != rotation)
			// {
			// 	game_object->GetDrawable()->SetRotation(Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));
			// }

			// previous_transforms_[game_object] = transform;
		}

		auto l = snprintf(nullptr, 0, "%zd", game_objects_list_.size());
		std::vector<char> buf(l + 1);
		snprintf(&buf[0], l + 1, "%zd", game_objects_list_.size());
		LOGVALUE("Entities count", std::string(buf.begin(), buf.end()));
	}

	SceneEventsHandler &Scene::GetSceneEventsHandler()
	{
		return scene_events_handler_;
	}
}