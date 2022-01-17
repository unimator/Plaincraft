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

#ifndef PLAINCRAFT_CORE_CHUNK
#define PLAINCRAFT_CORE_CHUNK

#include "../blocks/block.hpp"
#include <plaincraft_render_engine.hpp>
#include <array>

namespace plaincraft_core
{
    using namespace plaincraft_render_engine;

    class ModelsCache;

    class Chunk
    {
    public:
        static constexpr uint32_t chunk_size = 4;
        static constexpr uint32_t chunk_height = 8;

        static constexpr const char* chunk_model_name_template = "chunk_block_faces_%d%d%d%d%d%d";

        using Data = std::array<std::array<std::array<std::shared_ptr<Block>, chunk_size>, chunk_height>, chunk_size>;

    private:
        Data blocks_;

        int32_t pos_x_, pos_y_;
        std::shared_ptr<Drawable> mesh_;

    public:
        Chunk(int32_t position_x, int32_t position_y, Data&& blocks);
        Chunk(Chunk&& other) noexcept;

        void OrganizeMesh(std::unique_ptr<ModelsFactory>& models_factory, ModelsCache& models_cache);

        int32_t GetPositionX() const;
        int32_t GetPositionY() const;

        Data& GetData();
    };
}

#endif // PLAINCRAFT_CORE_CHUNK