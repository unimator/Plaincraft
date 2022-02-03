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
	bool less<std::reference_wrapper<plaincraft_core::GameObject>>::operator()(std::reference_wrapper<plaincraft_core::GameObject> first_entity,
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
		entities_list_.clear();
	}

	void Scene::AddGameObject(std::shared_ptr<GameObject> entity_to_add)
	{
		entities_list_.push_back(entity_to_add);

		auto object_type = entity_to_add->GetObjectType();
		if(object_type == GameObject::ObjectType::Dynamic)
		{
			dynamic_entities_list_.push_back(entity_to_add);
		}

		auto rigid_body = entity_to_add->GetRigidBody();
		if (rigid_body != nullptr && object_type == GameObject::ObjectType::Dynamic)
		{
			const rp3d::Transform &transform = entity_to_add->GetRigidBody()->getTransform();
			previous_transforms_.insert({entity_to_add, transform});
		}

		if (entity_to_add->GetDrawable() != nullptr)
		{
			render_engine_->AddDrawable(entity_to_add->GetDrawable());
		}
	}

	void Scene::RemoveGameObject(std::shared_ptr<GameObject> entity_to_remove)
	{
		auto remove_predicate = [&](std::shared_ptr<GameObject> const &game_object)
								 { return game_object == entity_to_remove; };
		
		entities_list_.remove_if(remove_predicate);

		auto object_type = entity_to_remove->GetObjectType();
		if(entity_to_remove->GetObjectType() == object_type)
		{
			dynamic_entities_list_.remove_if(remove_predicate);
		}

		auto rigid_body = entity_to_remove->GetRigidBody();
		if(rigid_body != nullptr && object_type == GameObject::ObjectType::Dynamic)
		{
			previous_transforms_.erase(previous_transforms_.find(entity_to_remove));
		}

		if(entity_to_remove->GetDrawable() != nullptr)
		{
			render_engine_->RemoveDrawable(entity_to_remove->GetDrawable());
		}
	}

	std::shared_ptr<GameObject> Scene::FindGameObjectByName(const std::string &name) const
	{
		auto predicate = [&](const std::shared_ptr<const GameObject> game_object)
		{ return game_object->GetName() == name; };
		auto result = std::find_if(begin(entities_list_), end(entities_list_), predicate);
		if (result != end(entities_list_))
		{
			return *result;
		}
		return nullptr;
	}

	void Scene::UpdateFrame(float interpolation_factor)
	{
		for (auto &game_object : dynamic_entities_list_)
		{
			const auto rb = game_object->GetRigidBody();

			if (rb == nullptr)
			{
				continue;
			}

			auto transform = rb->getTransform();
			auto previous_transform = previous_transforms_[game_object];
			auto interpolated_transform = rp3d::Transform::interpolateTransforms(previous_transform, transform, interpolation_factor);

			auto position = FromRP3D(interpolated_transform.getPosition());
			auto rotation = FromRP3D(interpolated_transform.getOrientation());

			auto drawable = game_object->GetDrawable();

			if(drawable->GetPosition() != position)
			{
				game_object->GetDrawable()->SetPosition(Vector3d(position.x, position.y, position.z));
			}

			if(drawable->GetRotation() != rotation)
			{
				game_object->GetDrawable()->SetRotation(Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));
			}

			previous_transforms_[game_object] = transform;
		}

		auto l = snprintf(nullptr, 0, "%zd", entities_list_.size());
		std::vector<char> buf(l + 1);
		snprintf(&buf[0], l + 1, "%zd", entities_list_.size());
		LOGVALUE("Entities count", std::string(buf.begin(), buf.end()));
	}
}