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

#ifndef PLAINCRAFT_CORE_WORLD_OPTIMIZER
#define PLAINCRAFT_CORE_WORLD_OPTIMIZER

#include "../entities/map/map.hpp"
#include "../entities/map/chunk.hpp"
#include "../assets/assets_manager.hpp"
#include <optional>
#include <functional>
#include <plaincraft_common.hpp>
#include <plaincraft_render_engine.hpp>

namespace plaincraft_core
{
    using namespace plaincraft_render_engine;

    class WorldOptimizer
    {
        std::shared_ptr<Map> map_;
        AssetsManager &assets_manager_;
        ModelsFactory &models_factory_;

    public:
        WorldOptimizer(std::shared_ptr<Map> map,
                       AssetsManager &assets_manager,
                       ModelsFactory &models_factory);

        void OptimizeChunk(Chunk &chunk);
        void DisposeChunk(Chunk &chunk);
    };
}

#endif // PLAINCRAFT_CORE_WORLD_OPTIMIZER