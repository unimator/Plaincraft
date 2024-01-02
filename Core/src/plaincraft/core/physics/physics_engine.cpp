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

#include "./physics_engine.hpp"
#include <cmath>

namespace plaincraft_core
{
    PhysicsEngine::PhysicsEngine(PhysicsSettings physics_settings, std::shared_ptr<Map> &map)
        : physics_settings_(physics_settings), map_(map) {}

    void PhysicsEngine::AddObject(std::shared_ptr<PhysicsObject> &physics_object)
    {
        physics_objects_.push_back(physics_object);
        if (physics_object->type == PhysicsObject::ObjectType::Dynamic)
        {
            dynamic_objects_.push_back(physics_object);
        }
    }

    void PhysicsEngine::RemoveObject(const std::shared_ptr<PhysicsObject> &physics_object)
    {
        auto find_predicate = [&](const std::shared_ptr<PhysicsObject> &object)
        {
            return object == physics_object;
        };
        physics_objects_.remove_if(find_predicate);
        if (physics_object->type == PhysicsObject::ObjectType::Dynamic)
        {
            dynamic_objects_.remove_if(find_predicate);
        }
    }

    void PhysicsEngine::Step(float time_step)
    {
        for (auto &physic_object : dynamic_objects_)
        {
            if (abs(time_step * physic_object->velocity.x * physic_object->friction) < abs(physic_object->velocity.x))
            {
                physic_object->velocity.x -= time_step * physic_object->velocity.x * physic_object->friction;
            }
            else
            {
                physic_object->velocity.x = 0.0f;
            }

            if (abs(time_step * physic_object->velocity.z * physic_object->friction) < abs(physic_object->velocity.z))
            {
                physic_object->velocity.z -= time_step * physic_object->velocity.z * physic_object->friction;
            }
            else
            {
                physic_object->velocity.z = 0.0f;
            }

            if (glm::length(physic_object->velocity) < 0.0001f)
            {
                physic_object->velocity = Vector3d(0.0f, 0.0f, 0.0f);
            }

            physic_object->is_grounded = false;

            for (size_t i = 0; i < 3; ++i)
            {
                auto adjusted_velocity = time_step * physic_object->velocity;

                auto predicted_move = physic_object->velocity * time_step;

                auto potential_collisions = FindPotentialCollisions(physic_object, adjusted_velocity);

                if (potential_collisions.empty())
                {
                    break;
                }

                auto compare = [](std::pair<float, Vector3d> &first, std::pair<float, Vector3d> &second)
                {
                    return first.first < second.first;
                };

                auto [entry_time, normal] = *(std::min_element(potential_collisions.begin(), potential_collisions.end(), compare));
                entry_time -= 0.001;

                if (normal.x)
                {
                    physic_object->velocity.x = 0.0f;
                    physic_object->position.x += adjusted_velocity.x * entry_time;
                }

                if (normal.y > 0.0f)
                {
                    physic_object->velocity.y = physic_object->velocity.y < 0.0f ? 0.0f : physic_object->velocity.y;
                    physic_object->position.y += adjusted_velocity.y * entry_time;
                    if(physic_object->velocity.y == 0.0f)
                    {
                        physic_object->is_grounded = true;
                    }
                }

                if (normal.z)
                {
                    physic_object->velocity.z = 0.0f;
                    physic_object->position.z += adjusted_velocity.z * entry_time;
                }
            }
            
            physic_object->position += physic_object->velocity * time_step;
            physic_object->velocity += time_step * physics_settings_.gravity;
        }
    }

    std::vector<std::pair<float, Vector3d>> PhysicsEngine::FindPotentialCollisions(std::shared_ptr<PhysicsObject> &tested_object, Vector3d adjusted_velocity)
    {
        auto blocks_to_test = FindBlocksToTestAABBCollision(tested_object, adjusted_velocity);

        auto &size = tested_object->size;
        auto &position = tested_object->position;

        auto did_collide = false;
        float collision_time = 1.0f;
        Vector3d normals = Vector3d(0.0f, 0.0f, 0.0f);
        std::vector<std::pair<float, Vector3d>> result;

        for (auto &block_to_test : blocks_to_test)
        {
            auto [block_collision_time, block_normals] = TestAABBBlockCollision(tested_object, block_to_test);

            if (block_normals == Vector3d(0.0f, 0.0f, 0.f))
            {
                continue;
            }
            result.push_back(std::make_pair<float, Vector3d>(std::move(block_collision_time), std::move(block_normals)));
        }

        return std::move(result);
    }

    std::pair<float, Vector3d> PhysicsEngine::TestAABBBlockCollision(std::shared_ptr<PhysicsObject> &tested_object, const std::shared_ptr<Block> &block)
    {
        auto block_physics_object = block->GetPhysicsObject();

        auto tested_object_position = tested_object->position;
        auto tested_object_size = tested_object->size;
        auto tested_object_velocity = tested_object->velocity;
        auto block_position = block_physics_object->position;
        auto block_size = block_physics_object->size;

        auto tested_object_min_x = tested_object_position.x - tested_object_size.x / 2;
        auto tested_object_max_x = tested_object_position.x + tested_object_size.x / 2;
        auto tested_object_min_y = tested_object_position.y - tested_object_size.y / 2;
        auto tested_object_max_y = tested_object_position.y + tested_object_size.y / 2;
        auto tested_object_min_z = tested_object_position.z - tested_object_size.z / 2;
        auto tested_object_max_z = tested_object_position.z + tested_object_size.z / 2;

        auto block_min_x = block_position.x - block_size.x / 2;
        auto block_max_x = block_position.x + block_size.x / 2;
        auto block_min_y = block_position.y - block_size.y / 2;
        auto block_max_y = block_position.y + block_size.y / 2;
        auto block_min_z = block_position.z - block_size.z / 2;
        auto block_max_z = block_position.z + block_size.z / 2;

        bool did_collide = false;

        float x_inv_entry, y_inv_entry, z_inv_entry;
        float x_inv_exit, y_inv_exit, z_inv_exit;

        if (tested_object_velocity.x > 0.0f)
        {
            x_inv_entry = block_min_x - tested_object_max_x;
            x_inv_exit = block_max_x - tested_object_min_x;
        }
        else
        {
            x_inv_entry = block_max_x - tested_object_min_x;
            x_inv_exit = block_min_x - tested_object_max_x;
        }

        if (tested_object_velocity.y > 0.0f)
        {
            y_inv_entry = block_min_y - tested_object_max_y;
            y_inv_exit = block_max_y - tested_object_min_y;
        }
        else
        {
            y_inv_entry = block_max_y - tested_object_min_y;
            y_inv_exit = block_min_y - tested_object_max_y;
        }

        if (tested_object_velocity.z > 0.0f)
        {
            z_inv_entry = block_min_z - tested_object_max_z;
            z_inv_exit = block_max_z - tested_object_min_z;
        }
        else
        {
            z_inv_entry = block_max_z - tested_object_min_z;
            z_inv_exit = block_min_z - tested_object_max_z;
        }

        float x_entry, y_entry, z_entry;
        float x_exit, y_exit, z_exit;

        if (tested_object_velocity.x == 0.0f)
        {
            x_entry = -std::numeric_limits<float>::infinity();
            x_exit = std::numeric_limits<float>::infinity();
        }
        else
        {
            x_entry = x_inv_entry / tested_object_velocity.x;
            x_exit = x_inv_exit / tested_object_velocity.x;
        }

        if (tested_object_velocity.y == 0.0f)
        {
            y_entry = -std::numeric_limits<float>::infinity();
            y_exit = std::numeric_limits<float>::infinity();
        }
        else
        {
            y_entry = y_inv_entry / tested_object_velocity.y;
            y_exit = y_inv_exit / tested_object_velocity.y;
        }

        if (tested_object_velocity.z == 0.0f)
        {
            z_entry = -std::numeric_limits<float>::infinity();
            z_exit = std::numeric_limits<float>::infinity();
        }
        else
        {
            z_entry = z_inv_entry / tested_object_velocity.z;
            z_exit = z_inv_exit / tested_object_velocity.z;
        }

        float entry_time = Max(x_entry, y_entry, z_entry);
        float exit_time = Min(x_exit, y_exit, z_exit);
        float normal_x = 0.0f, normal_y = 0.0f, normal_z = 0.0f;

        if (entry_time > exit_time || x_entry < 0.0f && y_entry < 0.0f && z_entry < 0.0f || x_entry > 1.0f || y_entry > 1.0f || z_entry > 1.0f)
        {
            normal_x = 0.0f;
            normal_y = 0.0f;
            normal_z = 0.0f;
            return std::make_pair<float, Vector3d>(1.0f, Vector3d(normal_x, normal_y, normal_z));
        }
        else
        {
            if (x_entry > y_entry && x_entry > z_entry)
            {
                if (x_inv_entry < 0.0f)
                {
                    normal_x = 1.0f;
                }
                else
                {
                    normal_x = -1.0f;
                }
            }
            else if (y_entry > x_entry && y_entry > z_entry)
            {
                if (y_inv_entry < 0.0f)
                {
                    normal_y = 1.0f;
                }
                else
                {
                    normal_y = -1.0f;
                }
            }
            else if (z_entry > x_entry && z_entry > y_entry)
            {
                if (z_inv_entry < 0.0f)
                {
                    normal_z = 1.0f;
                }
                else
                {
                    normal_z = -1.0f;
                }
            }
        }

        return std::make_pair<float, Vector3d>(std::move(entry_time), Vector3d(normal_x, normal_y, normal_z));
    }

    std::set<std::shared_ptr<Block>> PhysicsEngine::FindBlocksToTestAABBCollision(std::shared_ptr<PhysicsObject> &tested_object, Vector3d predicted_move)
    {
        auto &size = tested_object->size;
        auto &position = tested_object->position;
        auto radius = Max(size.x, size.y, size.z);

        auto block_size = Vector3d(1.0f, 1.0f, 1.0f);

        auto result = std::set<std::shared_ptr<Block>>();

        auto &grid = map_->GetGrid();
        auto grid_x = grid[0][0]->GetPositionX();
        auto grid_z = grid[0][0]->GetPositionZ();

        Vector3d broadPhaseBoxMin = Vector3d(
            predicted_move.x > 0 ? position.x : position.x + predicted_move.x,
            predicted_move.y > 0 ? position.y : position.y + predicted_move.y,
            predicted_move.z > 0 ? position.z : position.z + predicted_move.z
        ) - Vector3d(size.x / 2, size.y / 2, size.z / 2);

        Vector3d broadPhaseBoxMax = Vector3d(
            predicted_move.x > 0 ? position.x + predicted_move.x : position.x,
            predicted_move.y > 0 ? position.y + predicted_move.y : position.y,
            predicted_move.z > 0 ? position.z + predicted_move.z : position.z
        ) + Vector3d(size.x / 2, size.y / 2, size.z / 2);

        auto chunk_size = static_cast<int32_t>(Chunk::chunk_size);

        for(int32_t x = static_cast<int32_t>(position.x) - radius; x <= static_cast<int32_t>(position.x) + radius; ++x)
        {
            for(int32_t y = static_cast<int32_t>(position.y) - radius; y <= static_cast<int32_t>(position.y) + radius; ++y)
            {
                for(int32_t z = static_cast<int32_t>(position.z) - radius; z <= static_cast<int32_t>(position.z) + radius; ++z)
                {
                    int32_t x_grid_alligned = x < 0 ? (x - chunk_size + 1) / chunk_size : x / chunk_size;
                    int32_t z_grid_alligned = z < 0 ? (z - chunk_size + 1) / chunk_size : z / chunk_size;
                    auto chunk_x = x_grid_alligned - grid[0][0]->GetPositionX();
                    auto chunk_z = z_grid_alligned - grid[0][0]->GetPositionZ();

                    auto block_x = (x % chunk_size + chunk_size) % chunk_size;
                    auto block_y = y % Chunk::chunk_height;
                    auto block_z = (z % chunk_size + chunk_size) % chunk_size;

                    Vector3d block_position(x_grid_alligned * chunk_size + block_x, block_y, z_grid_alligned * chunk_size + block_z);

                    if(block_position.x + block_size.x / 2 < broadPhaseBoxMin.x || block_position.x - block_size.x / 2 > broadPhaseBoxMax.x
                        || block_position.y + block_size.y / 2 < broadPhaseBoxMin.y || block_position.y - block_size.y / 2 > broadPhaseBoxMax.y
                        || block_position.z + block_size.z / 2 < broadPhaseBoxMin.z || block_position.z - block_size.z / 2 > broadPhaseBoxMax.z
                    ) 
                    {
                        continue;
                    }

                    if(block_x < 0 || block_x >= chunk_size || block_y < 0 || block_y >= Chunk::chunk_height || block_z < 0 || block_z >= chunk_size)
                    {
                        printf("Warning: block out of range (%d, %d, %d)\n", block_x, block_y, block_z);
                        continue;
                    }

                    auto chunk = grid[chunk_x][chunk_z];
                    auto& block_data = chunk->GetData();

                    auto block = block_data[block_x][block_y][block_z];
                    if(block != nullptr)
                    {
                        result.insert(block);
                    }
                }
            }
        }
        return result;
    }
}