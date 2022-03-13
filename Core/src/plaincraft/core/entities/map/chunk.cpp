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

#include "chunk.hpp"
#include <iostream>
#include <set>

namespace plaincraft_core
{
    Chunk::Chunk(int32_t position_x, int32_t position_y, Data &&blocks)
        : pos_x_(position_x), pos_z_(position_y), blocks_(std::move(blocks))
    {
        InitializeName();
    }

    Chunk::Chunk(Chunk &&other) noexcept
        : GameObject(std::move(other)), pos_x_(other.pos_x_), pos_z_(other.pos_z_), blocks_(std::move(other.blocks_))
    {
        other.pos_x_ = 0;
        other.pos_z_ = 0;
    }

    Chunk &Chunk::operator=(Chunk &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        this->pos_x_ = other.pos_x_;
        this->pos_z_ = other.pos_z_;
        this->blocks_ = std::move(other.blocks_);

        return *this;
    }

    int32_t Chunk::GetPositionX() const
    {
        return pos_x_;
    }

    int32_t Chunk::GetPositionZ() const
    {
        return pos_z_;
    }

    Chunk::Data &Chunk::GetData()
    {
        return blocks_;
    }

    void Chunk::InitializeName()
    {
        auto size = std::snprintf(nullptr, 0, chunk_model_name_template, pos_x_, pos_z_) + 1;
        std::vector<char> buffer(size);
        std::snprintf(buffer.data(), size, chunk_model_name_template, pos_x_, pos_z_);
        SetName(std::string(buffer.begin(), buffer.end()));
    }
}