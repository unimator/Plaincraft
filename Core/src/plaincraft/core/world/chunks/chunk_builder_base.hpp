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

#ifndef PLAINCRAFT_CORE_CHUNK_BUILDER_BASE
#define PLAINCRAFT_CORE_CHUNK_BUILDER_BASE

#include "../../common.hpp"
#include "../../entities/map/chunk.hpp"

namespace plaincraft_core 
{
    class ChunkBuilderBase
    {
        public:
            virtual std::shared_ptr<Chunk> InitializeChunk(int32_t position_x, int32_t position_z);
            virtual bool GenerateChunkStep(std::shared_ptr<Chunk> chunk) = 0;
            virtual bool DisposeChunkStep(std::shared_ptr<Chunk> chunk) = 0;
    };
}

#endif // PLAINCRAFT_CORE_CHUNK_BUILDER_BASE