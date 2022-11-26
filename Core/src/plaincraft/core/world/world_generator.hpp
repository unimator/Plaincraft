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

#ifndef PLAINCRAFT_CORE_WORLD_GENERATOR
#define PLAINCRAFT_CORE_WORLD_GENERATOR

#include "../entities/map/map.hpp"
#include "../entities/game_object.hpp"
#include "../scene/scene.hpp"
#include "./world_optimizer.hpp"
#include "./chunks/chunk_builder.hpp"
#include "./chunks/chunks_processor.hpp"
#include <vector>
#include <functional>
#include <memory>
#include <tuple>

namespace plaincraft_core
{
    class WorldGenerator final
    {
        Scene &scene_;
        std::shared_ptr<Map> map_;
        std::shared_ptr<GameObject> origin_entity_;

        ChunksProcessor chunks_processor_;

    public:
        WorldGenerator(std::unique_ptr<WorldOptimizer> world_optimizer,
                     std::unique_ptr<ChunkBuilder> chunk_builder,
                     Scene &scene,
                     std::shared_ptr<Map> map,
                     std::shared_ptr<GameObject> origin_entity);

        void OnLoopFrameTick(float delta_time);

    private:
        void ReloadGrid();

        void Log();
    };
}

#endif // PLAINCRAFT_CORE_WORLD_GENERATOR