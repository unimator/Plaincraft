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

        const auto &lower_chunk = map_->grid_[0][0];
        const auto &higher_chunk = map_->grid_[Map::map_size - 1][Map::map_size - 1];
        int32_t lower_boundary_x = lower_chunk->GetPositionX() * Chunk::chunk_size;
        int32_t lower_boundary_y = lower_chunk->GetPositionZ() * Chunk::chunk_size;
        int32_t higher_boundary_x = higher_chunk->GetPositionX() * Chunk::chunk_size;
        int32_t higher_boundary_y = higher_chunk->GetPositionZ() * Chunk::chunk_size;

        size_t s = snprintf(nullptr, 0, "(%d, %d)", lower_boundary_x, lower_boundary_y);
        std::vector<char> buffer_lower(s + 1);
        snprintf(&buffer_lower[0], buffer_lower.size(), "(%d, %d)", lower_boundary_x, lower_boundary_y);
        Logger::LogValue("lower boundaries", std::string(buffer_lower.begin(), buffer_lower.end()));

        s = snprintf(nullptr, 0, "(%d, %d)", higher_boundary_x, higher_boundary_y);
        std::vector<char> buffer_higher(s + 1);
        snprintf(&buffer_higher[0], buffer_higher.size(), "(%d, %d)", higher_boundary_x, higher_boundary_y);
        Logger::LogValue("higher boundaries", std::string(buffer_higher.begin(), buffer_higher.end()));

        if (origin_position.x < static_cast<float>(lower_boundary_x) || origin_position.z < static_cast<float>(lower_boundary_y) || origin_position.x > static_cast<float>(higher_boundary_x) + Chunk::chunk_size || origin_position.z > static_cast<float>(higher_boundary_y) + Chunk::chunk_size)
        {
            ReloadGrid();
            world_optimizer_->OptimizeMap();
        }
    }

    void WorldUpdater::ReloadGrid()
    {
        auto origin_position = origin_entity_->GetRigidBody()->getTransform().getPosition() / Chunk::chunk_size;

        int32_t start_x = static_cast<int32_t>(origin_position.x) - Map::map_size / 2;
        int32_t start_z = static_cast<int32_t>(origin_position.z) - Map::map_size / 2;

        int32_t old_grid_start_x, old_grid_start_z;
        auto current_grid = map_->grid_;

        if (!current_grid.empty())
        {
            old_grid_start_x = current_grid[0][0]->GetPositionX();
            old_grid_start_z = current_grid[0][0]->GetPositionZ();
        }

        auto new_grid = Map::ChunksGrid(Map::map_size);

        auto current_x = start_x;
        auto current_z = start_z;

        for (auto &row : new_grid)
        {
            row = Map::ChunksRow(Map::map_size);

            current_z = start_z;

            for (auto &chunk : row)
            {
                if (!current_grid.empty() && current_x < old_grid_start_x + static_cast<int32_t>(Map::map_size) && current_x >= old_grid_start_x && current_z < old_grid_start_z + static_cast<int32_t>(Map::map_size) && current_z >= old_grid_start_z)
                {
                    chunk = std::move(current_grid[current_x - old_grid_start_x][current_z - old_grid_start_z]);
                }

                if (chunk == nullptr)
                {
                    chunk = std::make_shared<Chunk>(world_generator_->CreateChunk(I32Vector3d(current_x, 0, current_z)));
                    chunk->SetDrawable(std::make_shared<Drawable>());
                    scene_.AddGameObject(chunk);
                }

                ++current_z;
            }

            ++current_x;
        }

        if (!current_grid.empty())
        {
            for (auto &row : current_grid)
            {
                for (auto &chunk : row)
                {
                    if (chunk != nullptr)
                    {
                        world_generator_->DisposeChunk(chunk);
                    }
                }
            }
        }

        map_->grid_ = std::move(new_grid);
    }
}