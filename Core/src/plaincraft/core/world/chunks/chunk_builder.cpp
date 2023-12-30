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
#include "../../entities/blocks/dirt.hpp"
#include "../../entities/blocks/stone.hpp"
#include <plaincraft_common.hpp>
#include <plaincraft_render_engine.hpp>
#include <cmath>
#include <cstdint>
#include <random>

namespace plaincraft_core
{
	ChunkBuilder::ChunkBuilder(Scene &scene,
							   Cache<Model> &models_cache,
							   Cache<Texture> &textures_cache,
							   uint64_t seed)
		: scene_(scene),
		  models_cache_(models_cache),
		  textures_cache_(textures_cache),
		  seed_(seed),
		  perlin_(seed)
	{
		perlin_.reseed(seed);
	}
	
	bool ChunkBuilder::GenerateChunkStep(std::shared_ptr<Chunk> chunk)
	{
		// static auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5, 0.5, 0.5));

		auto &chunk_processing_data = GetProcessingData(chunk_creation_datas_, chunk);
		auto &[i, j, k] = chunk_processing_data;

		auto x = static_cast<int32_t>(Chunk::chunk_size) * chunk->GetPositionX() + i * 1.0;
		auto y = static_cast<int32_t>(Chunk::chunk_size) * 0 + j * 1.0;
		auto z = static_cast<int32_t>(Chunk::chunk_size) * chunk->GetPositionZ() + k * 1.0;
		auto noise = perlin_.normalizedOctave2D_01(x / 256, z / 256, 4);

		const uint32_t height = static_cast<uint32_t>(Chunk::chunk_height * noise);

		if (j <= height)
		{
			std::shared_ptr<Block> game_object;
			game_object = std::make_shared<Stone>();
			auto physics_object = std::make_shared<PhysicsObject>();
			physics_object->size = Vector3d(1.0f, 1.0f, 1.0f);
			physics_object->position = Vector3d(x, y, z);
			physics_object->type = PhysicsObject::ObjectType::Static;
			game_object->SetPhysicsObject(physics_object);
			// game_object->SetObjectType(GameObject::ObjectType::Static);
			chunk->blocks_[i][j][k] = game_object;
		}

		auto result = Increment(chunk_processing_data);

		if (result)
		{
			DisposeProcessingData(chunk_creation_datas_, chunk);
			chunk->initialized_ = true;
		}

		return result;
	}

	bool ChunkBuilder::DisposeChunkStep(std::shared_ptr<Chunk> chunk)
	{
		auto &chunk_processing_data = GetProcessingData(chunk_disposal_datas_, chunk);
		auto &[i, j, k] = chunk_processing_data;

		if (i == 0 && j == 0 && k == 0)
		{
			scene_.RemoveGameObject(chunk);
		}

		auto &blocks = chunk->GetData();
		if (blocks.empty())
		{
			DisposeProcessingData(chunk_disposal_datas_, chunk);
			return true;
		}

		auto &block = blocks[i][j][k];
		if (block != nullptr)
		{
			scene_.RemoveGameObject(block);
			// if(block->GetRigidBody())
			// {
			// 	physics_world_->destroyRigidBody(block->GetRigidBody());
			// }
		}

		auto result = Increment(chunk_processing_data);
		if (result)
		{
			DisposeProcessingData(chunk_disposal_datas_, chunk);
		}
		return result;
	}

	ChunkBuilder::ChunkProcessingData &ChunkBuilder::GetProcessingData(std::unordered_map<std::shared_ptr<Chunk>, ChunkBuilder::ChunkProcessingData> &collection, std::shared_ptr<Chunk> chunk)
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

	bool ChunkBuilder::Increment(ChunkProcessingData &chunk_processing_data)
	{
		auto &[i, j, k] = chunk_processing_data;

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
					return true;
				}
			}
		}

		return false;
	}
}
