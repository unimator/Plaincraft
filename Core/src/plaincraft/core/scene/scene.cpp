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
		std::lock_guard lg(scene_access_);
		game_objects_list_.clear();
		printf("Scene destroyed\n");
		fflush(stdout);
	}

	void Scene::AddGameObject(std::shared_ptr<GameObject> game_object_to_add)
	{
		std::lock_guard lg(scene_access_);
		game_objects_list_.push_back(game_object_to_add);

		if (game_object_to_add->GetDrawable() != nullptr)
		{
			render_engine_->AddDrawable(game_object_to_add->GetDrawable());
		}
	}

	void Scene::RemoveGameObject(std::shared_ptr<GameObject> game_object_to_remove)
	{
		std::lock_guard lg(scene_access_);
		auto remove_predicate = [&](std::shared_ptr<GameObject> const &game_object)
		{ return game_object == game_object_to_remove; };

		game_objects_list_.remove_if(remove_predicate);

		if (game_object_to_remove->GetDrawable() != nullptr)
		{
			render_engine_->RemoveDrawable(game_object_to_remove->GetDrawable());
		}
	}

	void Scene::RemoveGameObjects(std::vector<std::shared_ptr<GameObject>> &game_objects_to_remove)
	{
		std::lock_guard lg(scene_access_);
		std::unordered_map<std::shared_ptr<GameObject>, bool> game_objects_to_remove_presence_map;
		std::transform(game_objects_to_remove.begin(),
					   game_objects_to_remove.end(),
					   std::inserter(game_objects_to_remove_presence_map, game_objects_to_remove_presence_map.begin()),
					   [](const std::shared_ptr<GameObject> &game_object_to_remove)
					   { return std::make_pair(game_object_to_remove, true); });
		auto remove_predicate = [&](std::shared_ptr<GameObject> const &game_object)
		{ return game_objects_to_remove_presence_map.contains(game_object); };

		game_objects_list_.remove_if(remove_predicate);

		for (auto &game_object_to_remove : game_objects_to_remove)
		{
			if (game_object_to_remove->GetDrawable() != nullptr)
			{
				render_engine_->RemoveDrawable(game_object_to_remove->GetDrawable());
			}
		}
	}

	std::shared_ptr<GameObject> Scene::FindGameObjectByName(const std::string &name) const
	{
		std::lock_guard lg(scene_access_);
		auto predicate = [&](const std::shared_ptr<const GameObject> game_object)
		{ return game_object->GetName() == name; };
		auto result = std::find_if(begin(game_objects_list_), end(game_objects_list_), predicate);
		if (result != end(game_objects_list_))
		{
			return *result;
		}
		return nullptr;
	}

	void Scene::UpdateFrame(float interpolation_factor)
	{
		std::lock_guard lg(scene_access_);
		auto l = snprintf(nullptr, 0, "%zd", game_objects_list_.size());
		std::vector<char> buf(l + 1);
		snprintf(&buf[0], l + 1, "%zd", game_objects_list_.size());
		LOGVALUE("Entities count", std::string(buf.begin(), buf.end()));
	}

	void Scene::RenderFrame(plaincraft_render_engine::FrameConfig frame_config)
	{
		std::lock_guard lg(scene_access_);
		render_engine_->RenderFrame(frame_config);
	}

	SceneEventsHandler &Scene::GetSceneEventsHandler()
	{
		return scene_events_handler_;
	}
}