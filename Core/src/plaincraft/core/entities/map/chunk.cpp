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
#include <iostream>
#include <set>

namespace plaincraft_core
{
    Chunk::Chunk(int32_t position_x, int32_t position_y, Data &&blocks)
        : pos_x_(position_x), pos_z_(position_y), blocks_(std::move(blocks))
    {
        auto size = std::snprintf(nullptr, 0, "Chunk_%d_%d", position_x, position_y) + 1;
        std::vector<char> buffer(size);
        std::snprintf(buffer.data(), size, "Chunk_%d_%d", position_x, position_y);
        SetName(std::string(buffer.begin(), buffer.end()));
    }

    Chunk::Chunk(Chunk &&other) noexcept
        : GameObject(std::move(other)), pos_x_(other.pos_x_), pos_z_(other.pos_z_), blocks_(std::move(other.blocks_))
    {
        other.pos_x_ = 0;
        other.pos_z_ = 0;
    }

    Chunk &Chunk::operator=(Chunk &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        this->pos_x_ = other.pos_x_;
        this->pos_z_ = other.pos_z_;
        this->blocks_ = std::move(other.blocks_);

        return *this;
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
                             Cache<Model> &models_cache,
                             Cache<Texture> &textures_cache,
                             std::optional<std::reference_wrapper<Chunk>> negative_x_chunk,
                             std::optional<std::reference_wrapper<Chunk>> positive_x_chunk,
                             std::optional<std::reference_wrapper<Chunk>> negative_z_chunk,
                             std::optional<std::reference_wrapper<Chunk>> positive_z_chunk)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

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

                    auto txt_u_factor = 16.0f / 384.0f;
                    auto txt_v_factor = 16.0f / 544.0f;

                    // X axis check
                    if (x == 0 && negative_x_chunk.has_value() &&
                            negative_x_chunk.value().get().blocks_[chunk_size - 1][y][z] == nullptr ||
                        x > 0 && blocks_[x - 1][y][z] == nullptr)
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 1 * txt_v_factor}});
                    }

                    if (x == chunk_size - 1 && positive_x_chunk.has_value() &&
                            positive_x_chunk.value().get().blocks_[0][y][z] == nullptr ||
                        x < chunk_size - 1 && blocks_[x + 1][y][z] == nullptr)
                    {
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 0 * txt_v_factor}});
                    }

                    // Y axis check
                    if (y > 0 && blocks_[x][y - 1][z] == nullptr)
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.5f}});
                        vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
                    }
                    if (y < chunk_height - 1 && blocks_[x][y + 1][z] == nullptr)
                    {
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0 * txt_u_factor, 1 * txt_v_factor}});
                    }

                    // Z axis check
                    if ((z == 0 && negative_z_chunk.has_value() &&
                         negative_z_chunk.value().get().blocks_[x][y][chunk_size - 1] == nullptr) ||
                        (z > 0 && blocks_[x][y][z - 1] == nullptr) || true)
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {3 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {4 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {4 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {3 * txt_u_factor, 0 * txt_v_factor}});
                    }

                    if ((z == chunk_size - 1 && positive_z_chunk.has_value() &&
                         positive_z_chunk.value().get().blocks_[x][y][0] == nullptr) ||
                        (z < chunk_size - 1 && blocks_[x][y][z + 1] == nullptr) || true)
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {3 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {3 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {4 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {4 * txt_u_factor, 1 * txt_v_factor}});
                    }
                }
            }
        }

        for (auto i = 0; i < vertices.size() / 4; ++i)
        {
            indices.push_back(2 + 4 * i);
            indices.push_back(1 + 4 * i);
            indices.push_back(0 + 4 * i);

            indices.push_back(2 + 4 * i);
            indices.push_back(0 + 4 * i);
            indices.push_back(3 + 4 * i);
        }

        auto mesh = std::make_shared<plaincraft_render_engine::Mesh>(std::move(vertices), std::move(indices));
        if (models_cache.Contains(GetName()))
        {
            models_cache.Remove(GetName());
        }

        auto drawable_position_x = chunk_size * static_cast<float>(pos_x_);
        auto drawable_position_z = chunk_size * static_cast<float>(pos_z_);
        GetDrawable()->SetPosition(Vector3d(drawable_position_x, 0, drawable_position_z));
        GetDrawable()->SetColor(Vector3d(0.3, 0.1, 0.7));

        models_cache.Store(GetName(), std::move(models_factory->CreateModel(mesh)));
        auto model = models_cache.Fetch(GetName());
        auto minecraft_texture = textures_cache.Fetch("minecraft-texture");
        GetDrawable()->SetModel(model);
        GetDrawable()->SetTexture(minecraft_texture);
    }
}