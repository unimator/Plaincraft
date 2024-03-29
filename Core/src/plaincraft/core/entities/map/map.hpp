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

#include "../game_object.hpp"
#include "chunk.hpp"
#include <vector>

namespace plaincraft_core
{
    class WorldGenerator;
    class WorldOptimizer;

    class Map : public GameObject
    {
        friend class WorldGenerator;
        friend class WorldOptimizer;

    private:
        using ChunksRow = std::vector<std::shared_ptr<Chunk>>;
        using ChunksGrid = std::vector<ChunksRow>;

        bool is_initialized_ = false;

    public:

        static constexpr uint32_t render_radius = 4;
        static constexpr uint32_t render_diameter = render_radius * 2;
        static constexpr uint32_t simulation_radius = 2;

        Map();

        ChunksRow& operator[](int block_index);
        const ChunksGrid& GetGrid() const;

    private:
        ChunksGrid grid_;
    };
}

#endif // PLAINCRAFT_CORE_MAP