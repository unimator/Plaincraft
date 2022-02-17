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

#ifndef PLAINCRAFT_CORE_MAP
#define PLAINCRAFT_CORE_MAP

#include "chunk.hpp"
#include "../game_object.hpp"
#include "../../world/world_generator.hpp"
#include <vector>

namespace plaincraft_core
{
    class ChunkPhysicsOptimizationStrategy;

    class Map : public GameObject
    {
        friend class ChunkPhysicsOptimizationStrategy;

    public:
        using ChunksRow = std::vector<std::shared_ptr<Chunk>>;
        using ChunksGrid = std::vector<ChunksRow>;

        static constexpr uint32_t map_size = 3;

    private:
        ChunksGrid grid_;
        WorldGenerator world_generator_;

        std::shared_ptr<GameObject> origin_entity_;

    public:
        Map(WorldGenerator world_generator, std::shared_ptr<GameObject> origin_entity);

        void OnLoopTick(float delta_time);

    private:
        void ReloadGrid();
        void OptimizeMap();
    };
}

#endif // PLAINCRAFT_CORE_MAP