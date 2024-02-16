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

#ifndef PLAINCRAFT_CORE_PHYSICS_ENGINE
#define PLAINCRAFT_CORE_PHYSICS_ENGINE

#include "../entities/map/map.hpp"
#include "./physics_object.hpp"
#include <functional>
#include <list>
#include <utility>
#include <plaincraft_common.hpp>

namespace plaincraft_core
{
    using namespace plaincraft_common;

    class PhysicsEngine final
    {
    public:
        struct PhysicsSettings
        {
            Vector3d gravity = Vector3d(0.0f, -9.81f, 0.0f);
            float air_friction = 1.2f;
        };

    private:
        std::shared_ptr<Map> map_;
        std::list<std::shared_ptr<PhysicsObject>> physics_objects_;
        std::list<std::shared_ptr<PhysicsObject>> dynamic_objects_;
        PhysicsSettings physics_settings_;

    public:
        PhysicsEngine(PhysicsSettings physics_settings, std::shared_ptr<Map> &map);

        void AddObject(std::shared_ptr<PhysicsObject> &physic_object);
        void RemoveObject(const std::shared_ptr<PhysicsObject> &physic_object);

        void Step(float time_step);

    private:
        std::vector<std::pair<float, Vector3d>> FindPotentialCollisions(std::shared_ptr<PhysicsObject>& tested_object, Vector3d adjusted_velocity);
        std::pair<float, Vector3d> TestAABBBlockCollision(std::shared_ptr<PhysicsObject>& tested_object, const std::shared_ptr<Block>& block);
        std::set<std::shared_ptr<Block>> FindBlocksToTestAABBCollision(std::shared_ptr<PhysicsObject>& tested_object, Vector3d predicted_move);
    };
}

#endif // PLAINCRAFT_CORE_PHYSICS_ENGINE