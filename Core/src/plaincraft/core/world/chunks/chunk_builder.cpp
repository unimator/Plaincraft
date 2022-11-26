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

#include "chunk_builder.hpp"
#include "../../utils/conversions.hpp"
#include <plaincraft_common.hpp>
#include <plaincraft_render_engine.hpp>
#include <cmath>

namespace plaincraft_core
{
	ChunkBuilder::ChunkBuilder(rp3d::PhysicsCommon &physics_common,
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

	std::shared_ptr<Chunk> ChunkBuilder::InitializeChunk(int32_t position_x, int32_t position_z)
	{
		auto drawable = std::make_shared<Drawable>();
		auto chunk = std::make_shared<Chunk>(position_x, position_z);
		chunk->SetDrawable(drawable);
		return chunk;
	}

	bool ChunkBuilder::DisposeChunkStep(std::shared_ptr<Chunk> chunk)
	{
		auto &chunk_processing_data = GetProcessingData(chunk_disposal_datas_, chunk);
		auto &[i, j, k, is_finished] = chunk_processing_data;

		if (is_finished)
		{
			DisposeProcessingData(chunk_disposal_datas_, chunk);
			return true;
		}

		if (i == 0 && j == 0 && k == 0)
		{
			scene_.RemoveGameObject(chunk);
		}

		auto &blocks = chunk->GetData();
		if (blocks.empty())
		{
			return true;
		}

		auto &block = blocks[i][j][k];
		if (block != nullptr)
		{
			scene_.RemoveGameObject(block);
			if(block->GetRigidBody())
			{
				physics_world_->destroyRigidBody(block->GetRigidBody());
			}
		}
		
		return Increment(chunk_processing_data);
	}

	bool ChunkBuilder::GenerateChunkStep(std::shared_ptr<Chunk> chunk)
	{
		static auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5, 0.5, 0.5));
		
		auto &chunk_processing_data = GetProcessingData(chunk_creation_datas_, chunk);
		auto &[i, j, k, is_finished] = chunk_processing_data;

		if (is_finished)
		{
			DisposeProcessingData(chunk_creation_datas_, chunk);
			return true;
		}

		if (j <= 3)
		{
			auto game_object = std::make_shared<Block>(I32Vector3d(i, j, k));
			game_object->SetObjectType(GameObject::ObjectType::Static);
			// drawable->SetModel(model);
			// game_object->SetDrawable(drawable);

			// auto position = Vector3d(offset.x + i * 1.0f, round(2 * sin(i) * cos(j)), offset.z + j * 1.0f);
			auto x = static_cast<int32_t>(Chunk::chunk_size) * chunk->GetPositionX() + i * 1.0;
			auto y = static_cast<int32_t>(Chunk::chunk_size) * 0 + j * 1.0;
			auto z = static_cast<int32_t>(Chunk::chunk_size) * chunk->GetPositionZ() + k * 1.0;
			auto position = Vector3d(x, y, z);
			auto orientation = rp3d::Quaternion::identity();
			rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
			rp3d::RigidBody *rigid_body;
			rigid_body = physics_world_->createRigidBody(transform);
			auto collider = rigid_body->addCollider(cube_shape, transform);
			collider->getMaterial().setFrictionCoefficient(1.0f);
			rigid_body->setType(rp3d::BodyType::STATIC);
			rigid_body->setTransform(rp3d::Transform(FromGlm(position), orientation));
			game_object->SetRigidBody(rigid_body);
			rigid_body->setIsSleeping(true);

			chunk->blocks_[i][j][k] = game_object;
		}

		return Increment(chunk_processing_data);
	}

	ChunkBuilder::ChunkProcessingData& ChunkBuilder::GetProcessingData(std::unordered_map<std::shared_ptr<Chunk>, ChunkBuilder::ChunkProcessingData> &collection, std::shared_ptr<Chunk> chunk)
	{
		if (!collection.contains(chunk))
		{
			collection.emplace(chunk, ChunkProcessingData());
		}

		auto &chunk_processing_data = collection[chunk];

		return chunk_processing_data;
	}

	void ChunkBuilder::DisposeProcessingData(std::unordered_map<std::shared_ptr<Chunk>, ChunkBuilder::ChunkProcessingData> &collection, std::shared_ptr<Chunk> chunk)
	{
		collection.erase(chunk);
	}

	bool ChunkBuilder::Increment(ChunkProcessingData& chunk_processing_data)
	{
		auto &[i, j, k, is_finished] = chunk_processing_data;

		++i;
		if (i == Chunk::chunk_size)
		{
			i = 0;
			++j;

			if (j == Chunk::chunk_height)
			{
				j = 0;
				++k;

				if (k == Chunk::chunk_size)
				{
					is_finished = true;
					return true;
				}
			}
		}

		return false;
	}
}
