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

#include "./chunks_processor.hpp"
#include <iostream>

namespace plaincraft_core
{
    ChunksProcessor::Metric::Metric(std::shared_ptr<GameObject> origin)
        : origin_(origin) {}

    ChunksProcessor::Metric::Metric(const Metric &other)
    {
        origin_ = other.origin_;
    }

    ChunksProcessor::Metric::Metric(Metric &&other) noexcept
    {
        origin_ = std::move(other.origin_);
    }

    ChunksProcessor::Metric &ChunksProcessor::Metric::operator=(const Metric &other)
    {
        this->origin_ = other.origin_;

        return *this;
    }

    ChunksProcessor::Metric &ChunksProcessor::Metric::operator=(Metric &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        this->origin_ = std::move(other.origin_);

        return *this;
    }

    bool ChunksProcessor::Metric::Compare(const std::shared_ptr<Chunk> &first, const std::shared_ptr<Chunk> &second)
    {
        // auto position = origin_->GetRigidBody()->getTransform().getPosition();
        // auto first_position_x = first->GetPositionX() * static_cast<float>(Chunk::chunk_size);
        // auto first_position_z = first->GetPositionZ() * static_cast<float>(Chunk::chunk_size);
        // auto second_position_x = second->GetPositionX() * static_cast<float>(Chunk::chunk_size);
        // auto second_position_z = second->GetPositionZ() * static_cast<float>(Chunk::chunk_size);

        // auto first_distance_to_player_squared = (first_position_x - position.x) * (first_position_x - position.x) + (first_position_z - position.z) * (first_position_z - position.z);
        // auto second_distance_to_player_squared = (second_position_x - position.x) * (second_position_x - position.x) + (second_position_z - position.z) * (second_position_z - position.z);
        // return first_distance_to_player_squared > second_distance_to_player_squared;
        return false;
    }

    ChunksProcessor::ChunksProcessor(
        std::unique_ptr<ChunkBuilderBase> chunk_builder,
        std::unique_ptr<WorldOptimizer> world_optimizer,
        Scene &scene,
        Metric metric)
        : chunk_builder_(std::move(chunk_builder)),
          world_optimizer_(std::move(world_optimizer)),
          scene_(scene),
          metric_(metric)
    {
    }

    ChunksProcessor::ChunksProcessor(ChunksProcessor &&other) noexcept
        : metric_(std::move(other.metric_)),
          chunk_builder_(std::move(other.chunk_builder_)),
          world_optimizer_(std::move(other.world_optimizer_)),
          scene_(std::move(other.scene_)),
          requested_chunks_(std::move(other.requested_chunks_)),
          rejected_chunks_(std::move(other.rejected_chunks_)),
          current_to_create_(std::move(other.current_to_create_)),
          current_to_dispose_(std::move(other.current_to_dispose_))
    {
    }

    ChunksProcessor &ChunksProcessor::operator=(ChunksProcessor &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        this->chunk_builder_ = std::move(other.chunk_builder_);
        this->world_optimizer_ = std::move(other.world_optimizer_);
        this->scene_ = std::move(other.scene_);
        this->metric_ = std::move(other.metric_);
        this->requested_chunks_ = std::move(other.requested_chunks_);
        this->rejected_chunks_ = std::move(other.rejected_chunks_);
        this->current_to_create_ = std::move(other.current_to_create_);
        this->current_to_dispose_ = std::move(other.current_to_dispose_);

        return *this;
    }

    void ChunksProcessor::Process(uint32_t max_processing_steps)
    {
        float dispose_to_create_ratio = 0.5;
        uint32_t dispose_steps = max_processing_steps * dispose_to_create_ratio;
        uint32_t create_steps = max_processing_steps - dispose_steps;

        while (create_steps--)
        {
            CreationStep();
            if (requested_chunks_.empty() && current_to_create_ == nullptr)
        {
                break;
            }
        }

        while (dispose_steps--)
        {
            DisposalStep();
            if (rejected_chunks_.empty() && current_to_dispose_ == nullptr)
            {
                break;
            }
        }
    }

    std::shared_ptr<Chunk> ChunksProcessor::RequestChunk(int32_t chunk_x, int32_t chunk_z)
    {
        std::list<std::shared_ptr<Chunk>>::iterator chunk_iterator;
        std::shared_ptr<Chunk> result;

        auto find_chunk_predicate = [chunk_x, chunk_z](const std::shared_ptr<Chunk> &chunk)
        {
            return chunk->pos_x_ == chunk_x && chunk->pos_z_ == chunk_z;
        };

        // Check if chunk is already created
        if (current_to_create_ != nullptr && current_to_create_->pos_x_ == chunk_x && current_to_create_->pos_z_ == chunk_z)
        {
            return current_to_create_;
        }

        // Check if chunk is waiting to be processed
        chunk_iterator = std::find_if(requested_chunks_.begin(), requested_chunks_.end(), find_chunk_predicate);

        if (chunk_iterator != std::end(requested_chunks_))
        {
            return *chunk_iterator;
        }

        // Check if chunk is waiting to be rejected
        chunk_iterator = std::find_if(rejected_chunks_.begin(), rejected_chunks_.end(), find_chunk_predicate);

        if (chunk_iterator != std::end(rejected_chunks_))
        {
            result = *chunk_iterator;
            // If so it's no longer to be rejected
            rejected_chunks_.remove_if(find_chunk_predicate);
            return result;
        }

        // If chunk is neither to be processed nor rejected create it and put to be processed
        result = chunk_builder_->InitializeChunk(chunk_x, chunk_z);
        requested_chunks_.emplace_back(result);
        return result;
    }

    void ChunksProcessor::RejectChunk(std::shared_ptr<Chunk> chunk)
    {
        auto find_chunk_predicate = [&](const std::shared_ptr<Chunk> &chunk_arg)
        {
            return chunk == chunk_arg;
        };

        requested_chunks_.remove_if(find_chunk_predicate);

        // Only initialized chunks requires clean up
        if (chunk->initialized_)
        {
            rejected_chunks_.emplace_back(chunk);
        }
    }

    void ChunksProcessor::CreationStep()
    {
        current_to_create_ = current_to_create_ ? current_to_create_ : GetNextChunkToCreate();
        if (current_to_create_ == nullptr)
        {
            return;
        }

        if (chunk_builder_->GenerateChunkStep(current_to_create_))
        {
            world_optimizer_->OptimizeChunk(*current_to_create_);
            scene_.get().AddGameObject(current_to_create_);
            current_to_create_ = nullptr;
        }
    }

    void ChunksProcessor::DisposalStep()
    {
        current_to_dispose_ = current_to_dispose_ ? current_to_dispose_ : GetNextChunkToDispose();

        if (current_to_dispose_ == nullptr)
        {
            return;
        }

        if (chunk_builder_->DisposeChunkStep(current_to_dispose_))
        {
            world_optimizer_->DisposeChunk(*current_to_dispose_);
            current_to_dispose_ = nullptr;
        }
    }

    std::shared_ptr<Chunk> ChunksProcessor::GetNextChunkToCreate()
    {
        if (requested_chunks_.empty() || stop_processing)
        {
            return nullptr;
        }

        auto compare = [&](const std::shared_ptr<Chunk> &first, const std::shared_ptr<Chunk> &second) -> bool
        {
            return metric_.Compare(first, second);
        };

        requested_chunks_.sort(compare);
        auto result = requested_chunks_.back();
        requested_chunks_.pop_back();

        return result;
    }

    std::shared_ptr<Chunk> ChunksProcessor::GetNextChunkToDispose()
    {
        if (rejected_chunks_.empty() || stop_processing)
        {
            return nullptr;
        }

        auto result = rejected_chunks_.back();
        rejected_chunks_.pop_back();

        return result;
    }
}