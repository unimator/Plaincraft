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

#include "chunk.hpp"
#include "../../models/models_cache.hpp"
#include <set>
#include <iostream>

namespace plaincraft_core
{
    Chunk::Chunk(int32_t position_x, int32_t position_y, Data &&blocks)
        : pos_x_(position_x), pos_z_(position_y), blocks_(std::move(blocks))
    {
    }

    Chunk::Chunk(Chunk &&other) noexcept
        : pos_x_(other.pos_x_),
          pos_z_(other.pos_z_),
          blocks_(std::move(other.blocks_))
    {
        other.pos_x_ = 0;
        other.pos_z_ = 0;
    }

    int32_t Chunk::GetPositionX() const
    {
        return pos_x_;
    }

    int32_t Chunk::GetPositionZ() const
    {
        return pos_z_;
    }

    Chunk::Data &Chunk::GetData()
    {
        return blocks_;
    }

    void Chunk::OrganizeMesh(std::unique_ptr<ModelsFactory> &models_factory,
                             ModelsCache &models_cache,
                             std::optional<std::reference_wrapper<Chunk>> negative_x_chunk,
                             std::optional<std::reference_wrapper<Chunk>> positive_x_chunk,
                             std::optional<std::reference_wrapper<Chunk>> negative_z_chunk,
                             std::optional<std::reference_wrapper<Chunk>> positive_z_chunk)
    {
        for (auto x = 0; x < chunk_size; ++x)
        {
            for (auto y = 0; y < chunk_height; ++y)
            {
                for (auto z = 0; z < chunk_size; ++z)
                {
                    std::set<Cube::Faces> visible_faces;
                    auto block = blocks_[x][y][z];

                    if (block == nullptr)
                    {
                        continue;
                    }

                    // X axis check
                    if (x == 0 && negative_x_chunk.has_value() && negative_x_chunk.value().get().blocks_[chunk_size - 1][y][z] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::NegativeX);
                    }
                    else if (x > 0 && blocks_[x - 1][y][z] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::NegativeX);
                    }

                    if (x == chunk_size - 1 && positive_x_chunk.has_value() && positive_x_chunk.value().get().blocks_[0][y][z] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::PositiveX);
                    }
                    else if (x < chunk_size - 1 && blocks_[x + 1][y][z] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::PositiveX);
                    }

                    // Y axis check
                    if (y > 0 && blocks_[x][y - 1][z] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::NegativeY);
                    }
                    if (y < chunk_height - 1 && blocks_[x][y + 1][z] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::PositiveY);
                    }

                    // Z axis check
                    if (z == 0 && negative_z_chunk.has_value() && negative_z_chunk.value().get().blocks_[x][y][chunk_size - 1] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::NegativeZ);
                    }
                    else if (z > 0 && blocks_[x][y][z - 1] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::NegativeZ);
                    }

                    if (z == chunk_size - 1 && positive_z_chunk.has_value() && positive_z_chunk.value().get().blocks_[x][y][0] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::PositiveZ);
                    }
                    else if (z < chunk_size - 1 && blocks_[x][y][z + 1] == nullptr)
                    {
                        visible_faces.insert(Cube::Faces::PositiveZ);
                    }

                    auto cube_faces_hash = [](std::set<Cube::Faces> &visible_faces) -> std::string
                    {
                        std::vector<char> buffer(strlen(chunk_model_name_template) + 1);
                        sprintf(buffer.data(),
                                chunk_model_name_template,
                                visible_faces.contains(Cube::Faces::PositiveX),
                                visible_faces.contains(Cube::Faces::NegativeX),
                                visible_faces.contains(Cube::Faces::PositiveY),
                                visible_faces.contains(Cube::Faces::NegativeY),
                                visible_faces.contains(Cube::Faces::PositiveZ),
                                visible_faces.contains(Cube::Faces::NegativeZ));
                        return std::string(buffer.data());
                    };

                    auto drawable = block->GetDrawable();
                    if (visible_faces.size() == 0)
                    {
                        drawable->SetModel(nullptr);
                        continue;
                    }
                    auto old_model = drawable->GetModel().lock();

                    std::shared_ptr<plaincraft_render_engine::Model> new_model;
                    auto hashed_name = cube_faces_hash(visible_faces);
                    if (!models_cache.Contains(hashed_name))
                    {
                        auto cube = std::make_shared<plaincraft_render_engine::Cube>();
                        cube->FaceOptimize(visible_faces);
                        models_cache.Store(hashed_name, std::move(models_factory->CreateModel(std::move(cube), old_model->GetTexture())));
                    }
                    new_model = models_cache.Fetch(hashed_name);

                    drawable->SetModel(new_model);
                }
            }
        }
    }
}