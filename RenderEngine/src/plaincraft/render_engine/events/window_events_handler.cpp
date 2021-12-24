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

#include "window_events_handler.hpp"

namespace plaincraft_render_engine 
{
    WindowEventsHandler::WindowEventsHandler(GLFWwindow* instance)
    : instance_(instance)
    {
        glfwSetWindowUserPointer(instance_, this);

        glfwSetFramebufferSizeCallback(instance_, FrameBufferSizeCallbackWrapper);
        glfwSetKeyCallback(instance_, KeyCallbackWrapper);
    }

    WindowEventsHandler::~WindowEventsHandler()
    {
        if(instance_ == nullptr)
        {
            return;   
        }

        glfwSetWindowUserPointer(instance_, nullptr);

        glfwSetFramebufferSizeCallback(instance_, nullptr);
        glfwSetKeyCallback(instance_, nullptr);
    }

    WindowEventsHandler::WindowEventsHandler(WindowEventsHandler&& other) noexcept
        : instance_(other.instance_),
        window_resized_event_trigger(std::move(other.window_resized_event_trigger)),
        key_pressed_event_trigger(std::move(other.key_pressed_event_trigger))
    { 
        other.instance_ = nullptr;
    }
    
    WindowEventsHandler& WindowEventsHandler::operator=(WindowEventsHandler&& other) noexcept
    {
        if(this == &other) {
            return *this;
        }

        this->instance_ = other.instance_;
        this->window_resized_event_trigger = std::move(other.window_resized_event_trigger);
        this->key_pressed_event_trigger = std::move(other.key_pressed_event_trigger);

        return *this;
    }

    void WindowEventsHandler::FrameBufferSizeCallbackWrapper(GLFWwindow* instance, int width, int height)
    {
        auto event_handler = reinterpret_cast<WindowEventsHandler *>(glfwGetWindowUserPointer(instance));
		event_handler->window_resized_event_trigger.Trigger(width, height);
    }

    void WindowEventsHandler::KeyCallbackWrapper(GLFWwindow* instance, int key, int scancode, int action, int mods)
    {
        auto event_handler = reinterpret_cast<WindowEventsHandler *>(glfwGetWindowUserPointer(instance));
        event_handler->key_pressed_event_trigger.Trigger(key, scancode, action, mods);
    }
}