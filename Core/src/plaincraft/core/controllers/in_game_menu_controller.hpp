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

#ifndef PLAINCRAFT_CORE_IN_GAME_MENU_CONTROLLER
#define PLAINCRAFT_CORE_IN_GAME_MENU_CONTROLLER

#include <plaincraft_render_engine.hpp>
#include "../input/input_stack.hpp"
#include "../input/input_target.hpp"
#include "../state/global_state.hpp"
#include <memory>

namespace plaincraft_core
{
    using namespace plaincraft_render_engine;

    class InGameMenuController
    {
    private:
        InputTarget input_target_;
        InputTarget menu_input_target_;

        std::shared_ptr<Menu> in_game_menu_;

        std::shared_ptr<RenderEngine> render_engine_;
        GlobalState &global_state_;
        Cache<Font> &fonts_cache_;
        InputStack &input_stack_;

    public:
        InGameMenuController(
            std::shared_ptr<RenderEngine> render_engine,
            GlobalState &global_state,
            Cache<Font> &fonts_cache,
            InputStack &input_stack);

        InputTarget &GetInputTarget();

        void OpenMenu(int scancode, int action, int mods);
        void CloseMenu(int scancode, int action, int mods);

    private:
        void QuitCallback();
        void ResumeCallback();
    };
}

#endif // PLAINCRAFT_CORE_IN_GAME_MENU_CONTROLLER