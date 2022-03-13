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

#include "./world_updater.hpp"
#include <thread>
#include <mutex>

namespace plaincraft_core
{
    WorldUpdater::WorldUpdater(std::unique_ptr<WorldOptimizer> world_optimizer,
                               std::unique_ptr<WorldGenerator> world_generator,
                               Scene &scene,
                               std::shared_ptr<Map> map,
                               std::shared_ptr<GameObject> origin_entity)
        : world_optimizer_(std::move(world_optimizer)),
          world_generator_(std::move(world_generator)),
          scene_(scene),
          map_(map),
          origin_entity_(origin_entity)
    {
        ReloadGrid();
        world_optimizer_->OptimizeMap();
    }

    void WorldUpdater::OnLoopFrameTick(float delta_time)
    {
        auto origin_position = origin_entity_->GetRigidBody()->getTransform().getPosition();

        const auto &lower_chunk = map_->grid_[Map::render_radius - 1][Map::render_radius - 1];
        const auto &higher_chunk = map_->grid_[Map::render_radius][Map::render_radius];
        int32_t lower_boundary_x = lower_chunk->GetPositionX() * Chunk::chunk_size;
        int32_t lower_boundary_z = lower_chunk->GetPositionZ() * Chunk::chunk_size;
        int32_t higher_boundary_x = higher_chunk->GetPositionX() * Chunk::chunk_size;
        int32_t higher_boundary_z = higher_chunk->GetPositionZ() * Chunk::chunk_size;

        if (origin_position.x < static_cast<float>(lower_boundary_x) || origin_position.z < static_cast<float>(lower_boundary_z) || origin_position.x > static_cast<float>(higher_boundary_x) + Chunk::chunk_size || origin_position.z > static_cast<float>(higher_boundary_z) + Chunk::chunk_size)
        {
            ReloadGrid();
            world_optimizer_->OptimizeMap();
        }
    }

    void WorldUpdater::ReloadGrid()
    {
        auto origin_position = origin_entity_->GetRigidBody()->getTransform().getPosition() / Chunk::chunk_size;

        int32_t start_x = static_cast<int32_t>(origin_position.x) - Map::render_radius;
        int32_t start_z = static_cast<int32_t>(origin_position.z) - Map::render_radius;

        int32_t old_grid_start_x, old_grid_start_z;
        auto &current_grid = map_->grid_;

        if (map_->is_initialized_)
        {
            old_grid_start_x = current_grid[0][0]->GetPositionX();
            old_grid_start_z = current_grid[0][0]->GetPositionZ();
        }

        auto current_x = start_x;
        auto current_z = start_z;

        std::mutex chunk_mutex;

        auto regenerate_chunk = [&](std::shared_ptr<Chunk> &current_chunk, int32_t position_x, int32_t position_z) -> void
        {
            //std::lock_guard<std::mutex> guard(chunk_mutex);
            if (map_->is_initialized_ && position_x < old_grid_start_x + static_cast<int32_t>(Map::render_diameter) && position_x >= old_grid_start_x && position_z < old_grid_start_z + static_cast<int32_t>(Map::render_diameter) && position_z >= old_grid_start_z)
            {
                current_chunk = std::move(current_grid[position_x - old_grid_start_x][position_z - old_grid_start_z]);
            }
            else
            {
                if (current_chunk != nullptr)
                {
                    world_generator_->DisposeChunk(current_chunk);
                }
                current_chunk = std::make_shared<Chunk>(world_generator_->CreateChunk(I32Vector3d(current_x, 0, current_z)));
                current_chunk->SetDrawable(std::make_shared<Drawable>());
                scene_.AddGameObject(current_chunk);
            }
        };

        chunk_generation_futures_.clear();
        Map::ChunksGrid new_grid;
        new_grid = Map::ChunksGrid(Map::render_diameter);
        for (auto &row : new_grid)
        {
            row = Map::ChunksRow(Map::render_diameter);
        }

        for (auto &row : new_grid)
        {
            current_z = start_z;

            for (auto &chunk : row)
            {
                // chunk_generation_futures_.push_back(std::async(std::launch::async, regenerate_chunk, std::reference_wrapper<std::shared_ptr<Chunk>>(chunk), current_x, current_z));
                // regenerate_chunk(std::reference_wrapper<std::shared_ptr<Chunk>>(chunk), current_x, current_z);

                if (map_->is_initialized_ && current_x < old_grid_start_x + static_cast<int32_t>(Map::render_diameter) && current_x >= old_grid_start_x && current_z < old_grid_start_z + static_cast<int32_t>(Map::render_diameter) && current_z >= old_grid_start_z)
                {
                    chunk = std::move(current_grid[current_x - old_grid_start_x][current_z - old_grid_start_z]);
                }
                else
                {
                    chunk = std::make_shared<Chunk>(world_generator_->CreateChunk(I32Vector3d(current_x, 0, current_z)));
                    chunk->SetDrawable(std::make_shared<Drawable>());
                    scene_.AddGameObject(chunk);
                }

                ++current_z;
            }

            ++current_x;
        }

        for(auto &row: current_grid)
        {
            for (auto &chunk : row)
            {
                if(chunk != nullptr)
                {
                    world_generator_->DisposeChunk(chunk);
                }
            }
        }

        map_->grid_ = new_grid;

        for (auto &future : chunk_generation_futures_)
        {
            future.wait();
        }

        map_->is_initialized_ = true;
    }
}