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
#include <cmath>

namespace plaincraft_core
{
	WorldGenerator::WorldGenerator(rp3d::PhysicsCommon &physics_common,
								   std::shared_ptr<rp3d::PhysicsWorld> physics_world,
								   std::shared_ptr<RenderEngine> render_engine,
								   Scene &scene,
								   Cache<Model> &models_cache,
								   Cache<Texture> &textures_cache)
		: physics_common_(physics_common),
		  physics_world_(physics_world),
		  render_engine_(render_engine),
		  scene_(scene),
		  models_cache_(models_cache),
		  textures_cache_(textures_cache)
	{
	}

	Chunk WorldGenerator::CreateChunk(I32Vector3d offset)
	{
		// const auto model = models_cache_.Fetch("cube_half");

		auto chunk_data = Chunk::Data();
		auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5, 0.5, 0.5));

		std::shared_ptr<Block> game_object;

		for (auto i = 0; i < Chunk::chunk_size; ++i)
		{
			for (auto j = 0; j < Chunk::chunk_height; ++j)
			{
				if (j > 3)
				{
					continue;
				}

				for (auto k = 0; k < Chunk::chunk_size; ++k)
				{
					if ((rand() % 100) < 35)
					{
						continue;
					}

					auto game_object = std::make_shared<Block>(I32Vector3d(i, j, k));
					game_object->SetObjectType(GameObject::ObjectType::Static);
					auto drawable = std::make_shared<Drawable>();
					// drawable->SetModel(model);
					// game_object->SetDrawable(drawable);

					// auto position = Vector3d(offset.x + i * 1.0f, round(2 * sin(i) * cos(j)), offset.z + j * 1.0f);
					auto x = static_cast<int32_t>(Chunk::chunk_size) * offset.x + i * 1.0;
					auto y = static_cast<int32_t>(Chunk::chunk_size) * offset.y + j * 1.0;
					auto z = static_cast<int32_t>(Chunk::chunk_size) * offset.z + k * 1.0;
					auto position = Vector3d(x, y, z);
					auto orientation = rp3d::Quaternion::identity();
					rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
					auto rigid_body = physics_world_->createRigidBody(transform);
					auto collider = rigid_body->addCollider(cube_shape, transform);
					collider->getMaterial().setFrictionCoefficient(1.0f);
					rigid_body->setType(rp3d::BodyType::STATIC);
					rigid_body->setTransform(rp3d::Transform(FromGlm(position), orientation));
					game_object->SetRigidBody(rigid_body);
					rigid_body->setIsSleeping(true);

					// game_object->SetColor(color);

					scene_.AddGameObject(game_object);

					// game_object->GetDrawable()->SetPosition(Vector3d(position.x, position.y, position.z));
					// game_object->GetDrawable()->SetRotation(Quaternion(orientation.w, orientation.x, orientation.y, orientation.z));

					chunk_data[i][j][k] = game_object;
				}
			}
		}

		auto position_x = static_cast<int32_t>(offset.x);
		auto position_z = static_cast<int32_t>(offset.z);

		return Chunk(position_x, position_z, std::move(chunk_data));
	}

	void WorldGenerator::DisposeChunk(std::shared_ptr<Chunk> chunk)
	{
		auto &blocks = chunk->GetData();
		std::vector<std::shared_ptr<GameObject>> game_objects_to_remove;
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
							game_objects_to_remove.push_back(block);
							block->GetRigidBody()->setIsActive(false);
							free_rigid_bodies_.push(block->GetRigidBody());
							//physics_world_->destroyRigidBody(block->GetRigidBody());
						}
					}
				}
			}
		}
		game_objects_to_remove.push_back(chunk);
		scene_.RemoveGameObjects(game_objects_to_remove);
	}
}
