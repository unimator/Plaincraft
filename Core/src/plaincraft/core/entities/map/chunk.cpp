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

namespace plaincraft_core
{
    Chunk::Chunk(int32_t position_x, int32_t position_y, Data &&blocks)
        : pos_x_(position_x), pos_y_(position_y), blocks_(std::move(blocks))
    {
    }

    Chunk::Chunk(Chunk &&other) noexcept
        : pos_x_(other.pos_x_),
          pos_y_(other.pos_y_),
          blocks_(std::move(other.blocks_))
    {
        other.pos_x_ = 0;
        other.pos_y_ = 0;
    }

    int32_t Chunk::GetPositionX() const
    {
        return pos_x_;
    }

    int32_t Chunk::GetPositionY() const
    {
        return pos_y_;
    }

    Chunk::Data& Chunk::GetData()
    {
        return blocks_;
    }
}