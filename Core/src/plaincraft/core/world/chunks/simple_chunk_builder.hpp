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

#ifndef PLAINCRAFT_CORE_SIMPLE_CHUNK_BUILDER
#define PLAINCRAFT_CORE_SIMPLE_CHUNK_BUILDER

#include "chunk_builder_base.hpp"
#include "../../scene/scene.hpp"

namespace plaincraft_core
{
    class SimpleChunkBuilder : public ChunkBuilderBase
    {
    private:
        Scene &scene_;

    public:
        SimpleChunkBuilder(Scene &scene);

        bool GenerateChunkStep(std::shared_ptr<Chunk> chunk) override;
        bool DisposeChunkStep(std::shared_ptr<Chunk> chunk) override;
    };
}

#endif // PLAINCRAFT_CORE_SIMPLE_CHUNK_BUILDER