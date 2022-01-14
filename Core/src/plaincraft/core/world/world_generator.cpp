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

#include "world_generator.hpp"
#include "../utils/conversions.hpp"
#include <plaincraft_common.hpp>
#include <plaincraft_render_engine.hpp>
#include <plaincraft_render_engine_opengl.hpp>
#include <cmath>

namespace plaincraft_core
{
	WorldGenerator::WorldGenerator(rp3d::PhysicsCommon &physics_common,
								   std::shared_ptr<rp3d::PhysicsWorld> physics_world,
								   std::shared_ptr<RenderEngine> render_engine,
								   Scene &scene,
								   ModelsCache &models_cache)
		: physics_common_(physics_common),
		  physics_world_(physics_world),
		  render_engine_(render_engine),
		  scene_(scene),
		  models_cache_(models_cache)
	{
	}

	Chunk WorldGenerator::CreateChunk(Vector3d offset)
	{
		const auto model = models_cache_.Fetch("cube_half");

		auto chunk_data = Chunk::Data();
		auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5, 0.5, 0.5));

		int32_t i = 0, j = 0;
		for (auto &plane : chunk_data)
		{
			for (auto &row : plane)
			{
				for (auto &block : row)
				{
					auto entity = std::make_shared<Block>();
					auto drawable = std::make_shared<Drawable>();
					drawable->SetModel(model);
					entity->SetDrawable(drawable);

					// auto position = Vector3d(offset.x + i * 1.0f, round(2 * sin(i) * cos(j)), offset.z + j * 1.0f);
					auto position = Vector3d(Chunk::chunk_size * offset.x + i * 1.0f, round(2 * sin(offset.x) * cos(offset.z)), Chunk::chunk_size * offset.z + j * 1.0f);
					auto orientation = rp3d::Quaternion::identity();
					rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
					auto rigid_body = physics_world_->createRigidBody(transform);
					auto collider = rigid_body->addCollider(cube_shape, transform);
					rigid_body->setType(rp3d::BodyType::STATIC);
					rigid_body->enableGravity(false);
					rigid_body->setTransform(rp3d::Transform(FromGlm(position), orientation));
					entity->SetRigidBody(rigid_body);

					// body->SetPosition(Vector3d(i * 1.0f, sin(i * 0.25f) * cos(j * 0.25f), j * 1.0f));
					// float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					// float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					// float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					float r = static_cast<float>(offset.x * offset.x / 10.0f);
					float g = static_cast<float>(offset.y);
					float b = static_cast<float>(offset.z * offset.z / 10.0f);
					auto color = glm::vec3(r, g, b);
					entity->SetColor(color);

					scene_.AddEntity(entity);

					block = entity;
					
					++j;
				}
				++i;
			}
			break;
		}

		auto position_x = static_cast<int32_t>(offset.x);
		auto position_y = static_cast<int32_t>(offset.z);

		return Chunk(position_x, position_y, std::move(chunk_data));
	}

	void WorldGenerator::DisposeChunk(std::unique_ptr<Chunk> chunk)
	{
		auto& blocks = chunk->GetData();
		if (!blocks.empty())
        {
            for (auto &plane : blocks)
            {
                for (auto &row : plane)
                {
                    for (auto &block : row)
                    {
                        if(block != nullptr)
                        {
                            scene_.RemoveEntity(block);
							physics_world_->destroyRigidBody(block->GetRigidBody());
                        }
                    }
                }
            }
        }
	}
}
