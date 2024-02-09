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

#ifndef PLAINCRAFT_RENDER_ENGINE_MENU
#define PLAINCRAFT_RENDER_ENGINE_MENU

#include "menu_button.hpp"
#include "../gui_widget.hpp"
#include "../font/font.hpp"
#include <vector>
#include <functional>

namespace plaincraft_render_engine
{
    class Menu : public GuiWidget
    {
    private:
        std::vector<std::unique_ptr<MenuButton>> buttons_;
        std::shared_ptr<Font> font_;

        uint32_t position_x_;
        uint32_t position_y_;
        uint32_t width_;
        uint32_t height_;

    public:
        Menu();

        void AddButton(std::unique_ptr<MenuButton> button);

        std::vector<std::unique_ptr<MenuButton>> &GetButtons();

        void SetFont(std::shared_ptr<Font> font);
        std::shared_ptr<Font> GetFont() const;

        void SetPositionX(uint32_t position_x);
        uint32_t GetPositionX() const;

        void SetPositionY(uint32_t position_y);
        uint32_t GetPositionY() const;

        void SetWidth(uint32_t width);
        uint32_t GetWidth() const;

        void SetHeight(uint32_t height);
        uint32_t GetHeight() const;        
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_MENU