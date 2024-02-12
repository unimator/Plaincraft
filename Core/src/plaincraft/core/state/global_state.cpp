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

#include "global_state.hpp"

namespace plaincraft_core 
{
    void GlobalState::SetDebugInfoVisibility(bool is_visible)
    {
        is_debug_info_visible_ = is_visible;
    }

    bool GlobalState::GetDebugInfoVisibility() const 
    {
        return is_debug_info_visible_;
    }

    void GlobalState::SetSeed(uint64_t seed)
    {
        seed_ = seed;
    }

    uint64_t GlobalState::GetSeed() const
    {
        return seed_;
    }

    void GlobalState::SetIsRunning(bool is_running)
    {
        is_running_ = is_running;
    }

    bool GlobalState::GetIsRunning() const
    {
        return is_running_;
    }
}