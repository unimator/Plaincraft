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

#include "map.hpp"
#include <iostream>
#include <cmath>
#include <vector>

namespace plaincraft_core
{
    Map::Map(WorldGenerator world_generator, std::shared_ptr<Entity> origin_entity)
        : world_generator_(world_generator),
          origin_entity_(origin_entity)
    {
        ReloadGrid();
        OptimizeChunks();
    }

    void Map::OnLoopTick(float delta_time)
    {
        auto origin_position = origin_entity_->GetRigidBody()->getTransform().getPosition();

        const auto &lower_chunk = grid_[0][0];
        const auto &higher_chunk = grid_[map_size - 1][map_size - 1];
        int32_t lower_boundary_x = lower_chunk->GetPositionX() * Chunk::chunk_size;
        int32_t lower_boundary_y = lower_chunk->GetPositionY() * Chunk::chunk_size;
        int32_t higher_boundary_x = higher_chunk->GetPositionX() * Chunk::chunk_size;
        int32_t higher_boundary_y = higher_chunk->GetPositionY() * Chunk::chunk_size;

        size_t s = snprintf(nullptr, 0, "(%d, %d)", lower_boundary_x, lower_boundary_y);
        std::vector<char> buffer_lower(s + 1);
        snprintf(&buffer_lower[0], buffer_lower.size(), "(%d, %d)", lower_boundary_x, lower_boundary_y);
        Logger::LogValue("lower boundaries", std::string(buffer_lower.begin(), buffer_lower.end()));

        s = snprintf(nullptr, 0, "(%d, %d)", higher_boundary_x, higher_boundary_y);
        std::vector<char> buffer_higher(s + 1);
        snprintf(&buffer_higher[0], buffer_higher.size(), "(%d, %d)", higher_boundary_x, higher_boundary_y);
        Logger::LogValue("higher boundaries", std::string(buffer_higher.begin(), buffer_higher.end()));

        if(origin_position.x < static_cast<float>(lower_boundary_x)  
        || origin_position.z < static_cast<float>(lower_boundary_y)  
        || origin_position.x > static_cast<float>(higher_boundary_x) + Chunk::chunk_size 
        || origin_position.z > static_cast<float>(higher_boundary_y) + Chunk::chunk_size)
        {
            ReloadGrid();
            OptimizeChunks();
        }
    }

    void Map::ReloadGrid()
    {
        auto origin_position = origin_entity_->GetRigidBody()->getTransform().getPosition() / Chunk::chunk_size;

        int32_t start_x = static_cast<int32_t>(origin_position.x) - map_size / 2;
        int32_t start_y = static_cast<int32_t>(origin_position.z) - map_size / 2;

        int32_t old_grid_start_x, old_grid_start_y;

        if (!grid_.empty())
        {
            old_grid_start_x = grid_[0][0]->GetPositionX();
            old_grid_start_y = grid_[0][0]->GetPositionY();
        }

        auto new_grid = ChunksGrid(map_size);

        auto current_x = start_x;
        auto current_y = start_y;

        for (auto &row : new_grid)
        {
            row = ChunksRow(map_size);

            current_y = start_y;

            for (auto &chunk : row)
            {
                if (!grid_.empty() 
                    && current_x < old_grid_start_x + static_cast<int32_t>(map_size) && current_x >= old_grid_start_x 
                    && current_y < old_grid_start_y + static_cast<int32_t>(map_size) && current_y >= old_grid_start_y)
                {
                    chunk = std::move(grid_[current_x - old_grid_start_x][current_y - old_grid_start_y]);
                }

                if (chunk == nullptr)
                {
                    chunk = std::make_unique<Chunk>(world_generator_.CreateChunk(I32Vector3d(current_x, 0, current_y)));
                }

                ++current_y;
            }

            ++current_x;
        }

        if (!grid_.empty())
        {
            for(auto& row: grid_)
            {
                for(auto& chunk: row)
                {
                    if(chunk != nullptr)
                    {
                        world_generator_.DisposeChunk(std::move(chunk));
                    }
                }
            }
        }

        grid_ = std::move(new_grid);
    }

    void Map::OptimizeChunks()
    {
        for(auto& row: grid_)
        {
            for(auto& chunk: row)
            {
                chunk->OrganizeMesh(world_generator_.render_engine_->GetModelsFactory(), world_generator_.models_cache_);
            }
        }
    }
}