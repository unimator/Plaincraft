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

#include "window.hpp"
#include <iostream>

namespace plaincraft_render_engine
{
    Window::Window(std::string title, uint32_t width, uint32_t height)
        : instance_(nullptr), title_(title), width_(width), height_(height)
    {
    }

    Window::~Window()
    {
    }

    Window::Window(Window &&other)
    {
        this->width_ = other.width_;
        this->height_ = other.height_;
        this->title_ = std::move(other.title_);
        this->instance_ = other.instance_;
        other.instance_ = nullptr;
    }

    Window& Window::operator=(Window &&other)
    {
        this->width_ = other.width_;
        this->height_ = other.height_;
        this->title_ = other.title_;
        other.title_ = nullptr;
        this->instance_ = other.instance_;
        other.instance_ = nullptr;
        return *this;
    }

    WindowEventsHandler& Window::GetWindowEventsHandler()
    {
        return *window_events_handler_;
    }

    uint32_t Window::GetWidth() const
    {
        return width_;
    }

    uint32_t Window::GetHeight() const
    {
        return height_;
    }
}