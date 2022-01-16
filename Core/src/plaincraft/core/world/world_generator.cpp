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
#include <random>

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

	Chunk WorldGenerator::CreateChunk(I32Vector3d offset)
	{
		const auto model = models_cache_.Fetch("cube_half");

		auto chunk_data = Chunk::Data();
		auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5, 0.5, 0.5));

		std::shared_ptr<Block> entity;
		// auto entity = std::make_shared<Block>(offset + I32Vector3d(0, 0, 1));

		// auto create_block = [&](I32Vector3d relative_position) -> std::shared_ptr<Block>
		// {
		// 	auto entity = std::make_shared<Block>(offset + relative_position);
		// 	auto drawable = std::make_shared<Drawable>();
		// 	drawable->SetModel(model);
		// 	entity->SetDrawable(drawable);

		// 	// auto position = Vector3d(offset.x + i * 1.0f, round(2 * sin(i) * cos(j)), offset.z + j * 1.0f);
		// 	auto x = static_cast<int32_t>(Chunk::chunk_size) * offset.x + relative_position.x * 1.0;
		// 	auto y = static_cast<int32_t>(Chunk::chunk_size) * offset.y + relative_position.y * 1.0;
		// 	auto z = static_cast<int32_t>(Chunk::chunk_size) * offset.z + relative_position.z * 1.0;
		// 	auto position = Vector3d(x, y, z);
		// 	auto orientation = rp3d::Quaternion::identity();
		// 	rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
		// 	auto rigid_body = physics_world_->createRigidBody(transform);
		// 	auto collider = rigid_body->addCollider(cube_shape, transform);
		// 	rigid_body->setType(rp3d::BodyType::STATIC);
		// 	rigid_body->enableGravity(false);
		// 	rigid_body->setTransform(rp3d::Transform(FromGlm(position), orientation));
		// 	entity->SetRigidBody(rigid_body);

		// 	// body->SetPosition(Vector3d(i * 1.0f, sin(i * 0.25f) * cos(j * 0.25f), j * 1.0f));
		// 	// float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		// 	// float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		// 	// float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		// 	float r = static_cast<float>((rand() % 100) / 100.0f);
		// 	float g = static_cast<float>((rand() % 100) / 100.0f);
		// 	float b = static_cast<float>((rand() % 100) / 100.0f);
		// 	auto color = glm::vec3(r, g, b);
		// 	entity->SetColor(color);

		// 	scene_.AddEntity(entity);
		// 	chunk_data[relative_position.x][relative_position.y][relative_position.z] = entity;

		// 	return entity;
		// };

		// create_block(I32Vector3d(0, 0, 0));
		// create_block(I32Vector3d(0, 0, Chunk::chunk_size - 1));
		// create_block(I32Vector3d(0, Chunk::chunk_height - 1, 0));
		// create_block(I32Vector3d(Chunk::chunk_size - 1, 0, 0));
		// create_block(I32Vector3d(Chunk::chunk_size - 1, 0, Chunk::chunk_size - 1));
		// create_block(I32Vector3d(0, Chunk::chunk_height - 1, Chunk::chunk_size - 1));
		// // create_block(I32Vector3d(Chunk::chunk_size - 1, Chunk::chunk_height - 1, 0));
		// // create_block(I32Vector3d(Chunk::chunk_size - 1, Chunk::chunk_height - 1, Chunk::chunk_size - 1));

		// create_block(I32Vector3d(1, 2, 2));
		// create_block(I32Vector3d(3, 2, 2));
		// create_block(I32Vector3d(4, 2, 2));
		// create_block(I32Vector3d(5, 2, 2));
		// create_block(I32Vector3d(2, 1, 2));
		// create_block(I32Vector3d(2, 3, 2));
		// create_block(I32Vector3d(2, 2, 1));
		// create_block(I32Vector3d(2, 2, 3));
		// create_block(I32Vector3d(2, 2, 4));
		// create_block(I32Vector3d(2, 2, 5));
		// create_block(I32Vector3d(2, 2, 2));

		float r = static_cast<float>((rand() % 255) / 255.0f);
		float g = static_cast<float>((rand() % 255) / 255.0f);
		float b = static_cast<float>((rand() % 255) / 255.0f);
		auto color = glm::vec3(r, g, b);

		for (auto i = 0; i < Chunk::chunk_size; ++i)
		{
			for (auto j = 0; j < Chunk::chunk_height; ++j)
			{
				if (j > 2)
				{
					continue;
				}

				for (auto k = 0; k < Chunk::chunk_size; ++k)
				{
					if ((rand() % 100) < 65)
					{
						continue;
					}

					auto entity = std::make_shared<Block>(offset + I32Vector3d(i, j, k));
					auto drawable = std::make_shared<Drawable>();
					drawable->SetModel(model);
					entity->SetDrawable(drawable);

					// auto position = Vector3d(offset.x + i * 1.0f, round(2 * sin(i) * cos(j)), offset.z + j * 1.0f);
					auto x = static_cast<int32_t>(Chunk::chunk_size) * offset.x + i * 1.0;
					auto y = static_cast<int32_t>(Chunk::chunk_size) * offset.y + j * 1.0;
					auto z = static_cast<int32_t>(Chunk::chunk_size) * offset.z + k * 1.0;
					auto position = Vector3d(x, y, z);
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

					entity->SetColor(color);

					scene_.AddEntity(entity);

					chunk_data[i][j][k] = entity;
				}
			}
		}

		auto position_x = static_cast<int32_t>(offset.x);
		auto position_y = static_cast<int32_t>(offset.z);

		return Chunk(position_x, position_y, std::move(chunk_data));
	}

	void WorldGenerator::DisposeChunk(std::unique_ptr<Chunk> chunk)
	{
		auto &blocks = chunk->GetData();
		if (!blocks.empty())
		{
			for (auto &plane : blocks)
			{
				for (auto &row : plane)
				{
					for (auto &block : row)
					{
						if (block != nullptr)
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
