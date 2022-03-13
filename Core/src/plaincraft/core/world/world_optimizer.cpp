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

#include "./world_optimizer.hpp"

namespace plaincraft_core
{
    WorldOptimizer::WorldOptimizer(std::shared_ptr<Map> map,
                                   Cache<Model> &models_cache,
                                   Cache<Texture> &textures_cache,
                                   std::shared_ptr<ModelsFactory> models_factory)
        : map_(map), models_cache_(models_cache), textures_cache_(textures_cache), models_factory_(models_factory)
    {
    }

    void WorldOptimizer::OptimizeMap()
    {
        auto &current_grid = map_->grid_;

        for (size_t x = 0; x < Map::render_diameter; ++x)
        {
            for (size_t z = 0; z < Map::render_diameter; ++z)
            {
                auto &chunk = current_grid[x][z];

                std::optional<std::reference_wrapper<Chunk>> negative_x_chunk;
                std::optional<std::reference_wrapper<Chunk>> positive_x_chunk;
                std::optional<std::reference_wrapper<Chunk>> negative_z_chunk;
                std::optional<std::reference_wrapper<Chunk>> positive_z_chunk;

                if (x > 0)
                {
                    negative_x_chunk = *(current_grid[x - 1][z]);
                }
                if (x < Map::render_diameter - 1)
                {
                    positive_x_chunk = *(current_grid[x + 1][z]);
                }

                if (z > 0)
                {
                    negative_z_chunk = *(current_grid[x][z - 1]);
                }
                if (z < Map::render_diameter - 1)
                {
                    positive_z_chunk = *(current_grid[x][z + 1]);
                }

                OptimizeChunk(*chunk,
                              negative_x_chunk,
                              positive_x_chunk,
                              negative_z_chunk,
                              positive_z_chunk);
            }
        }
    }

    void WorldOptimizer::OptimizeChunk(Chunk &chunk,
                                       std::optional<std::reference_wrapper<Chunk>> negative_x_chunk,
                                       std::optional<std::reference_wrapper<Chunk>> positive_x_chunk,
                                       std::optional<std::reference_wrapper<Chunk>> negative_z_chunk,
                                       std::optional<std::reference_wrapper<Chunk>> positive_z_chunk)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        float r = static_cast<float>((rand() % 255) / 255.0f);
		float g = static_cast<float>((rand() % 255) / 255.0f);
		float b = static_cast<float>((rand() % 255) / 255.0f);
		auto color = glm::vec3(r, g, b);

        for (auto x = 0; x < Chunk::chunk_size; ++x)
        {
            for (auto y = 0; y < Chunk::chunk_height; ++y)
            {
                for (auto z = 0; z < Chunk::chunk_size; ++z)
                {
                    std::set<Cube::Faces> visible_faces;
                    auto block = chunk.blocks_[x][y][z];

                    if (block == nullptr)
                    {
                        continue;
                    }

                    auto txt_u_factor = 16.0f / 384.0f;
                    auto txt_v_factor = 16.0f / 544.0f;

                    // X axis check
                    if (x == 0 && negative_x_chunk.has_value() &&
                            negative_x_chunk.value().get().blocks_[Chunk::chunk_size - 1][y][z] == nullptr ||
                        x > 0 && chunk.blocks_[x - 1][y][z] == nullptr)
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 1 * txt_v_factor}});
                    }

                    if (x == Chunk::chunk_size - 1 && positive_x_chunk.has_value() &&
                            positive_x_chunk.value().get().blocks_[0][y][z] == nullptr ||
                        x < Chunk::chunk_size - 1 && chunk.blocks_[x + 1][y][z] == nullptr)
                    {
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {4 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {3 * txt_u_factor, 0 * txt_v_factor}});
                    }

                    // Y axis check
                    if (y > 0 && chunk.blocks_[x][y - 1][z] == nullptr)
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.5f}});
                        vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
                    }
                    if (y < Chunk::chunk_height - 1 && chunk.blocks_[x][y + 1][z] == nullptr)
                    {
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0 * txt_u_factor, 1 * txt_v_factor}});
                    }

                    // Z axis check
                    if ((z == 0 && negative_z_chunk.has_value() &&
                         negative_z_chunk.value().get().blocks_[x][y][Chunk::chunk_size - 1] == nullptr) ||
                        (z > 0 && chunk.blocks_[x][y][z - 1] == nullptr) || true)
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {3 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {4 * txt_u_factor, 1 * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {4 * txt_u_factor, 0 * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {3 * txt_u_factor, 0 * txt_v_factor}});
                    }

                    if ((z == Chunk::chunk_size - 1 && positive_z_chunk.has_value() &&
                         positive_z_chunk.value().get().blocks_[x][y][0] == nullptr) ||
                        (z < Chunk::chunk_size - 1 && chunk.blocks_[x][y][z + 1] == nullptr) || true)
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
        if (models_cache_.Contains(chunk.GetName()))
        {
            models_cache_.Remove(chunk.GetName());
        }

        auto drawable_position_x = Chunk::chunk_size * static_cast<float>(chunk.pos_x_);
        auto drawable_position_z = Chunk::chunk_size * static_cast<float>(chunk.pos_z_);
        chunk.GetDrawable()->SetPosition(Vector3d(drawable_position_x, 0, drawable_position_z));
        chunk.GetDrawable()->SetColor(color);

        models_cache_.Store(chunk.GetName(), std::move(models_factory_->CreateModel(mesh)));
        auto model = models_cache_.Fetch(chunk.GetName());
        auto minecraft_texture = textures_cache_.Fetch("minecraft-texture");
        chunk.GetDrawable()->SetModel(model);
        chunk.GetDrawable()->SetTexture(minecraft_texture);
    }
}