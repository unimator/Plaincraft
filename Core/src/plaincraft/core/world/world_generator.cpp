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

#include "./world_generator.hpp"
#include <thread>
#include <algorithm>
#include <cmath>

namespace plaincraft_core
{
    WorldGenerator::WorldGenerator(std::unique_ptr<WorldOptimizer> world_optimizer,
                                   std::unique_ptr<ChunkBuilderBase> world_generator,
                                   Scene &scene,
                                   std::shared_ptr<Map> map,
                                   std::shared_ptr<GameObject> origin_entity)
        : scene_(scene),
          map_(map),
          origin_entity_(origin_entity),
          chunks_processor_(std::move(world_generator), std::move(world_optimizer), scene, ChunksProcessor::Metric(origin_entity))
    {
        ReloadGrid();
        chunks_processor_.Process(INT32_MAX);
    }

    void WorldGenerator::OnLoopFrameTick(float delta_time)
    {
        auto origin_position = origin_entity_->GetPhysicsObject()->position;

        const auto &lower_chunk = map_->grid_[Map::render_radius - 1][Map::render_radius - 1];
        const auto &higher_chunk = map_->grid_[Map::render_radius][Map::render_radius];
        int32_t lower_boundary_x = lower_chunk->GetPositionX() * Chunk::chunk_size;
        int32_t lower_boundary_z = lower_chunk->GetPositionZ() * Chunk::chunk_size;
        int32_t higher_boundary_x = (higher_chunk->GetPositionX() + 1) * Chunk::chunk_size;
        int32_t higher_boundary_z = (higher_chunk->GetPositionZ() + 1) * Chunk::chunk_size;

        if (origin_position.x < static_cast<float>(lower_boundary_x) || origin_position.z < static_cast<float>(lower_boundary_z) || origin_position.x > static_cast<float>(higher_boundary_x) || origin_position.z > static_cast<float>(higher_boundary_z))
        {
            chunks_processor_.stop_processing = true;
            if (chunks_processor_.current_to_create_ == nullptr && chunks_processor_.current_to_dispose_ == nullptr)
            {
                ReloadGrid();
            }
        }
        else
        {
            chunks_processor_.stop_processing = false;
        }

        chunks_processor_.Process(200);
    }

    void WorldGenerator::ReloadGrid()
    {
        auto origin_position = origin_entity_->GetPhysicsObject()->position / static_cast<float>(Chunk::chunk_size);

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
                // Try to reuse chunk from the old grid
                if (map_->is_initialized_ && current_x < old_grid_start_x + static_cast<int32_t>(Map::render_diameter) && current_x >= old_grid_start_x && current_z < old_grid_start_z + static_cast<int32_t>(Map::render_diameter) && current_z >= old_grid_start_z)
                {
                    chunk = std::move(current_grid[current_x - old_grid_start_x][current_z - old_grid_start_z]);
                }
                else
                {
                    chunk = chunks_processor_.RequestChunk(current_x, current_z);
                }
                ++current_z;
            }

            ++current_x;
        }

        for (auto &row : current_grid)
        {
            for (auto &chunk : row)
            {
                if (chunk != nullptr)
                {
                    chunks_processor_.RejectChunk(chunk);
                }
            }
        }

        map_->grid_ = std::move(new_grid);
        map_->is_initialized_ = true;
    }
}