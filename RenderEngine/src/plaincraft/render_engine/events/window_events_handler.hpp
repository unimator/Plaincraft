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

#ifndef PLAINCRAFT_RENDER_ENGINE_WINDOW_EVENTS_HANDLER
#define PLAINCRAFT_RENDER_ENGINE_WINDOW_EVENTS_HANDLER

#include "../common.hpp"

namespace plaincraft_render_engine {
    class WindowEventsHandler final
    {
    private:
        GLFWwindow *instance_;

    public:
        plaincraft_common::EventTrigger<int, int> window_resized_event_trigger;
        plaincraft_common::EventTrigger<int, int, int, int> key_pressed_event_trigger;

        WindowEventsHandler(GLFWwindow *instance);
        
        WindowEventsHandler(const WindowEventsHandler& other) = delete;
        WindowEventsHandler& operator=(const WindowEventsHandler& other) = delete;

        WindowEventsHandler(WindowEventsHandler&& other) noexcept;
        WindowEventsHandler& operator=(WindowEventsHandler&& other) noexcept;

        ~WindowEventsHandler();

    private:
        static void FrameBufferSizeCallbackWrapper(GLFWwindow *instance, int width, int height);
        static void KeyCallbackWrapper(GLFWwindow *instance, int key, int scancode, int action, int mods);
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_WINDOW_EVENTS_HANDLER