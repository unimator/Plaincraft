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

#ifndef PLAINCRAFT_CORE_FPS_COUNTER
#define PLAINCRAFT_CORE_FPS_COUNTER

#include "../events/loop_events_handler.hpp"

namespace plaincraft_core
{
    class FpsCounter final
    {
        private:
            uint32_t frame_count_ = 0;
            float time_passed_ = 0.0f;

            uint32_t frames_per_second_ = 0;

        public:
            FpsCounter(LoopEventsHandler& loop_events_handler);
            ~FpsCounter();
            
            uint32_t GetFramesPerSecond() const;

        private:
            void OnLoopTick(float delta_time);
    };
}

#endif // PLAINCRAFT_CORE_FPS_COUNTER