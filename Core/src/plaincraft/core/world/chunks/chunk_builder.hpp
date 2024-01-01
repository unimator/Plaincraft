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

#ifndef PLAINCRAFT_CORE_CHUNK_BUILDER
#define PLAINCRAFT_CORE_CHUNK_BUILDER

#include "chunk_builder_base.hpp"
#include "../../common.hpp"
#include "../../scene/scene.hpp"
#include <lib/PerlinNoise.hpp>
#include <stack>
#include <unordered_map>
#include <mutex>

namespace plaincraft_core
{
	class ChunkBuilder : public ChunkBuilderBase
	{
	private:
		struct ChunkProcessingData
		{
			size_t i, j, k;
		};

		uint64_t seed_;
		siv::PerlinNoise perlin_;

	public:
		Scene &scene_;

		std::unordered_map<std::shared_ptr<Chunk>, ChunkProcessingData> chunk_creation_datas_;
		std::unordered_map<std::shared_ptr<Chunk>, ChunkProcessingData> chunk_disposal_datas_;
		std::mutex chunk_creation_datas_mutex_;
		std::mutex chunk_disposal_datas_mutex_;

	public:
		ChunkBuilder(Scene &scene, uint64_t seed);
		virtual ~ChunkBuilder();

		bool GenerateChunkStep(std::shared_ptr<Chunk> chunk) override;
		bool DisposeChunkStep(std::shared_ptr<Chunk> chunk) override;

	private:
		ChunkProcessingData &GetProcessingData(std::unordered_map<std::shared_ptr<Chunk>, ChunkProcessingData> &collection, std::shared_ptr<Chunk> chunk);
		void DisposeProcessingData(std::unordered_map<std::shared_ptr<Chunk>, ChunkProcessingData> &collection, std::shared_ptr<Chunk> chunk);
		bool Increment(ChunkProcessingData &chunk_processing_data);
	};
}

#endif // PLAINCRAFT_CORE_CHUNK_BUILDER
