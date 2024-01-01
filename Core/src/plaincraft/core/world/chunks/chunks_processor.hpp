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

#ifndef PLAINCRAFT_CORE_CHUNKS_PROCESSOR
#define PLAINCRAFT_CORE_CHUNKS_PROCESSOR

#include "../world_optimizer.hpp"
#include "../../entities/game_object.hpp"
#include "../../entities/map/chunk.hpp"
#include "../../scene/scene.hpp"
#include "./chunk_builder_base.hpp"
#include <list>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace plaincraft_core
{
    class ChunksProcessor
    {
    public:
        class Metric
        {
        private:
            std::shared_ptr<GameObject> origin_;

        public:
            Metric(std::shared_ptr<GameObject> origin);
            Metric(const Metric &other);
            Metric(Metric &&other) noexcept;
            Metric &operator=(const Metric &other);
            Metric &operator=(Metric &&other) noexcept;

            bool Compare(const std::shared_ptr<Chunk> &first, const std::shared_ptr<Chunk> &second);
        };

    public:
        std::atomic<bool> stop_processing = false;

        class WorldGenerator;
        friend class WorldGenerator;

    public:
        std::unique_ptr<ChunkBuilderBase> chunk_builder_;
        std::unique_ptr<WorldOptimizer> world_optimizer_;
        Metric metric_;
        std::reference_wrapper<Scene> scene_;

        std::list<std::shared_ptr<Chunk>> requested_chunks_;
        std::list<std::shared_ptr<Chunk>> rejected_chunks_;
        std::mutex requested_chunks_mutex_;
        std::mutex rejected_chunks_mutex_;

        std::shared_ptr<Chunk> current_to_create_ = nullptr;
        std::shared_ptr<Chunk> current_to_dispose_ = nullptr;

        std::thread creator_thread_;
        std::thread disposal_thread_;
        std::mutex creator_mutex_;
        std::mutex disposal_mutex_;
        std::condition_variable any_requested_;
        std::condition_variable any_rejected_;


    private:
        std::atomic<bool> stop_{false};

    public:
        ChunksProcessor(
            std::unique_ptr<ChunkBuilderBase> chunk_builder,
            std::unique_ptr<WorldOptimizer> world_optimizer,
            Scene &scene,
            Metric metric);

        ChunksProcessor(const ChunksProcessor &other) = delete;
        ChunksProcessor(ChunksProcessor &&other) noexcept;
        ChunksProcessor &operator=(const ChunksProcessor &other) = delete;
        ChunksProcessor &operator=(ChunksProcessor &&other) noexcept;

        ~ChunksProcessor();

        std::shared_ptr<Chunk> RequestChunk(int32_t chunk_x, int32_t chunk_z);
        void RejectChunk(std::shared_ptr<Chunk> chunk);

    private:
        std::shared_ptr<Chunk> GetNextChunkToCreate();
        std::shared_ptr<Chunk> GetNextChunkToDispose();

        void CreatorCallback();
        void DisposalCallback();
    };
}

#endif // PLAINCRAFT_CORE_CHUNKS_PROCESSOR
