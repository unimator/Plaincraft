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

#include "menu.hpp"
#include <functional>

namespace plaincraft_render_engine
{
    Menu::Menu()
    {
    }

    void Menu::AddButton(std::unique_ptr<MenuButton> button)
    {
        buttons_.push_back(std::move(button));
    }

    std::vector<std::unique_ptr<MenuButton>> &Menu::GetButtons()
    {
        return buttons_;
    }

    void Menu::SetFont(std::shared_ptr<Font> font)
    {
        font_ = font;
    }

    std::shared_ptr<Font> Menu::GetFont() const
    {
        return font_;
    }

    void Menu::SetPositionX(uint32_t position_x)
    {
        position_x_ = position_x;
    }

    uint32_t Menu::GetPositionX() const
    {
        return position_x_;
    }

    void Menu::SetPositionY(uint32_t position_y)
    {
        position_y_ = position_y;
    }

    uint32_t Menu::GetPositionY() const
    {
        return position_y_;
    }

    void Menu::SetWidth(uint32_t width)
    {
        width_ = width;
    }

    uint32_t Menu::GetWidth() const
    {
        return width_;
    }

    void Menu::SetHeight(uint32_t height)
    {
        height_ = height;
    }

    uint32_t Menu::GetHeight() const
    {
        return height_;
    }
}