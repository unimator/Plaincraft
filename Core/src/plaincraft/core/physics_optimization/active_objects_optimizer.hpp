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

#ifndef PLAINCRAFT_CORE_ACTIVE_OBJECTS_OPTIMIZER
#define PLAINCRAFT_CORE_ACTIVE_OBJECTS_OPTIMIZER

namespace plaincraft_core 
{
    class ActiveObjectsOptimizer;
}

#include "../entities/game_object.hpp"
#include "../scene/events/scene_events_handler.hpp"
#include "../entities/map/map.hpp"
#include "map_optimizer.hpp"
#include <list>
#include <memory>

namespace plaincraft_core
{
    class ActiveObjectsOptimizer
    {
    private:
        std::list<std::shared_ptr<GameObject>> &game_objects_list_;
        std::list<std::shared_ptr<GameObject>> &static_game_objects_list_;
        std::list<std::shared_ptr<GameObject>> &dynamic_game_objects_list_;

        MapOptimizer map_optimizer_;

    public:
        ActiveObjectsOptimizer(
            std::list<std::shared_ptr<GameObject>> &game_objects_list,
            std::list<std::shared_ptr<GameObject>> &static_game_objects_list,
            std::list<std::shared_ptr<GameObject>> &dynamic_game_objects_list);

        ~ActiveObjectsOptimizer();

        void OnLoopFrameTick(float delta_time);

    private:
        void Initialize();

        static std::shared_ptr<Map> FindMap(std::list<std::shared_ptr<GameObject>> &game_objects_list);
    };
}

#endif // PLAINCRAFT_CORE_ACTIVE_OBJECTS_OPTIMIZER