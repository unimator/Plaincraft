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

    void WorldOptimizer::OptimizeChunk(Chunk &chunk)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        float r = static_cast<float>(1.f);
		float g = static_cast<float>(1.f);
		float b = static_cast<float>(1.f);
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
                    auto& text_cood = block->GetTextureCoordinates();
                    auto& [top, bottom, left, right, front, back] = text_cood;

                    // X axis check
                    if (x == 0 || (x > 0 && chunk.blocks_[x - 1][y][z] == nullptr))
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {left.first * txt_u_factor, (left.second + 1) * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {left.first * txt_u_factor, left.second * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {(left.first + 1) * txt_u_factor, left.second * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {(left.first + 1) * txt_u_factor, (left.second + 1) * txt_v_factor}});
                    }

                    if ((x == Chunk::chunk_size - 1) || x < Chunk::chunk_size - 1 && chunk.blocks_[x + 1][y][z] == nullptr)
                    {
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {right.first * txt_u_factor, (right.second + 1) * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {(right.first + 1) * txt_u_factor, (right.second + 1) * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {(right.first + 1) * txt_u_factor, right.second * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {right.first * txt_u_factor, right.second * txt_v_factor}});
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
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {top.first * txt_u_factor, top.second * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {(top.first + 1) * txt_u_factor, top.second * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {(top.first + 1) * txt_u_factor, (top.second + 1) * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {top.first * txt_u_factor, (top.second + 1) * txt_v_factor}});
                    }

                    // Z axis check
                    if (z == 0 || (z > 0 && chunk.blocks_[x][y][z - 1] == nullptr))
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {front.first * txt_u_factor, (front.second + 1) * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {(front.first + 1) * txt_u_factor, (front.second + 1) * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {(front.first + 1) * txt_u_factor, front.second * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {front.first * txt_u_factor, front.second * txt_v_factor}});
                    }

                    if (z == Chunk::chunk_size - 1 || (z < Chunk::chunk_size - 1 && chunk.blocks_[x][y][z + 1] == nullptr))
                    {
                        vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {back.first * txt_u_factor, (back.second + 1) * txt_v_factor}});
                        vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {back.first * txt_u_factor, back.second * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {(back.first + 1) * txt_u_factor, back.second * txt_v_factor}});
                        vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {(back.first + 1) * txt_u_factor, (back.second + 1) * txt_v_factor}});
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

    void WorldOptimizer::DisposeChunk(Chunk& chunk)
    {
        if (models_cache_.Contains(chunk.GetName()))
        {
            models_cache_.Remove(chunk.GetName());
        }
    }
}