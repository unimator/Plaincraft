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
	bool less<std::reference_wrapper<plaincraft_core::Entity>>::operator()(std::reference_wrapper<plaincraft_core::Entity> first_entity,
																		   std::reference_wrapper<plaincraft_core::Entity> second_entity) const
	{
		return first_entity.get().GetUniqueId() < second_entity.get().GetUniqueId();
	}
}

namespace plaincraft_core
{
	using namespace plaincraft_render_engine;

	Scene::Scene()
	{
	}

	Scene::~Scene() 
	{
	}

	void Scene::AddEntity(std::shared_ptr<Entity> entity, std::unique_ptr<RenderEngine> &render_engine)
	{
		entities_list_.push_back(entity);
		const rp3d::Transform &transform = entity->GetRigidBody()->getTransform();
		previous_transforms_.insert({entity, transform});
		render_engine->AddDrawable(entity->GetDrawable());
	}

	std::shared_ptr<Entity> Scene::FindEntityByName(const std::string &name) const
	{
		auto predicate = [&](const std::shared_ptr<const Entity> entity)
		{ return entity->GetName() == name; };
		auto result = std::find_if(begin(entities_list_), end(entities_list_), predicate);
		if (result != end(entities_list_))
		{
			return *result;
		}
		return nullptr;
	}

	void Scene::UpdateFrame(float interpolation_factor)
	{
		for (auto entity : entities_list_)
		{
			const auto rb = entity->GetRigidBody();
			auto transform = rb->getTransform();
			auto previous_transform = previous_transforms_[entity];
			auto interpolated_transform = rp3d::Transform::interpolateTransforms(previous_transform, transform, interpolation_factor);

			auto position = interpolated_transform.getPosition();
			auto rotation = interpolated_transform.getOrientation();
			entity->GetDrawable()->SetPosition(Vector3d(position.x, position.y, position.z));
			entity->GetDrawable()->SetRotation(Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));

			previous_transforms_[entity] = transform;
		}
	}
}