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
        std::list<std::shared_ptr<GameObject>> &dynamic_game_objects_list)
        : game_objects_list_(game_objects_list),
          static_game_objects_list_(static_game_objects_list),
          dynamic_game_objects_list_(dynamic_game_objects_list),
          map_optimizer_(FindMap(game_objects_list), static_game_objects_list, dynamic_game_objects_list)
    {
        Initialize();
    }

    ActiveObjectsOptimizer::~ActiveObjectsOptimizer()
    {
    }

    void ActiveObjectsOptimizer::OnLoopFrameTick(float delta_time)
    {
        for (auto &static_entity : static_game_objects_list_)
        {
            if (static_entity->GetRigidBody() != nullptr)
            {
                static_entity->GetRigidBody()->setIsSleeping(true);
            }
        }
    }

    void ActiveObjectsOptimizer::Initialize()
    {
    }

    std::shared_ptr<Map> ActiveObjectsOptimizer::FindMap(std::list<std::shared_ptr<GameObject>> &game_objects_list)
    {
        auto predicate = [&game_objects_list](std::shared_ptr<GameObject> &current)
        {
            return std::dynamic_pointer_cast<Map>(current) != nullptr;
        };
        auto map_it = std::find_if(game_objects_list.begin(), game_objects_list.end(), predicate);
        return std::dynamic_pointer_cast<Map>(*map_it);
    }
}