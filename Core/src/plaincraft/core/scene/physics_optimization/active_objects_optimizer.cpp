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

#include "./active_objects_optimizer.hpp"

namespace plaincraft_core
{
    ActiveObjectsOptimizer::ActiveObjectsOptimizer(
        std::list<std::shared_ptr<GameObject>> &game_objects_list,
        std::list<std::shared_ptr<GameObject>> &static_game_objects_list,
        std::list<std::shared_ptr<GameObject>> &dynamic_game_objects_list,
        SceneEventsHandler &scene_events_handler)
        : game_objects_list_(game_objects_list),
          static_game_objects_list_(static_game_objects_list),
          dynamic_game_objects_list_(dynamic_game_objects_list),
          scene_events_handler_(scene_events_handler)
    {
        Initialize();
    }

    ActiveObjectsOptimizer::~ActiveObjectsOptimizer()
    {
        scene_events_handler_.on_add_object_event_trigger.RemoveSubscription(this);
        scene_events_handler_.on_remove_object_event_trigger.RemoveSubscription(this);
    }

    void ActiveObjectsOptimizer::Optimize()
    {
        for (auto &static_entity : static_game_objects_list_)
        {
            if (static_entity->GetRigidBody() != nullptr)
            {
                static_entity->GetRigidBody()->setIsSleeping(true);
            }
        }

        MapOptimize();
    }

    void ActiveObjectsOptimizer::Initialize()
    {
        scene_events_handler_.on_add_object_event_trigger.AddSubscription(this, &ActiveObjectsOptimizer::OnAddGameObjectToScene);
        scene_events_handler_.on_remove_object_event_trigger.AddSubscription(this, &ActiveObjectsOptimizer::OnRemoveGameObjectFromScene);
    }

    void ActiveObjectsOptimizer::OnAddGameObjectToScene(std::shared_ptr<GameObject> added_game_object)
    {
        if (added_game_object->GetObjectType() == GameObject::ObjectType::Static)
        {
            static_game_objects_list_.push_back(added_game_object);
        }
        else if (added_game_object->GetObjectType() == GameObject::ObjectType::Dynamic)
        {
            dynamic_game_objects_list_.push_back(added_game_object);
        }
    }

    void ActiveObjectsOptimizer::OnRemoveGameObjectFromScene(std::shared_ptr<GameObject> removed_game_object)
    {
        auto predicate = [&](std::shared_ptr<GameObject> game_object)
        {
            return game_object == removed_game_object;
        };
        if (removed_game_object->GetObjectType() == GameObject::ObjectType::Static)
        {
            auto game_object_it = std::find_if(static_game_objects_list_.begin(), static_game_objects_list_.end(), predicate);
            static_game_objects_list_.erase(game_object_it);
        }
        else if (removed_game_object->GetObjectType() == GameObject::ObjectType::Dynamic)
        {
            auto game_object_it = std::find_if(dynamic_game_objects_list_.begin(), dynamic_game_objects_list_.end(), predicate);
            dynamic_game_objects_list_.erase(game_object_it);
        }
    }

    void ActiveObjectsOptimizer::MapOptimize()
    {
    }
}