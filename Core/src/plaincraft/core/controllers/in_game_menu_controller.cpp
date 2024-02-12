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

#include "in_game_menu_controller.hpp"
#include <functional>

namespace plaincraft_core
{
    InGameMenuController::InGameMenuController(
        std::shared_ptr<RenderEngine> render_engine,
        Cache<Font> &fonts_cache,
        InputStack &input_stack)
        : input_target_(InputTarget::TargetType::Passive),
          menu_input_target_(InputTarget::TargetType::Blocking),
          render_engine_(render_engine),
          fonts_cache_(fonts_cache),
          input_stack_(input_stack)
    {
        input_target_.key_mappings[GLFW_KEY_ESCAPE].AddSubscription(this, &InGameMenuController::OpenMenu);
        menu_input_target_.key_mappings[GLFW_KEY_ESCAPE].AddSubscription(this, &InGameMenuController::CloseMenu);
    }

    InputTarget &InGameMenuController::GetInputTarget()
    {
        return input_target_;
    }

    void InGameMenuController::OpenMenu(int scancode, int action, int mods)
    {
        if(action != GLFW_PRESS)
        {
            return;
        }

        input_stack_.Push(menu_input_target_);

        auto &menu_factory = render_engine_->GetMenuFactory();
        in_game_menu_ = menu_factory->CreateMenu();
        in_game_menu_->SetFont(fonts_cache_.Fetch("standard"));
        in_game_menu_->AddButton(std::make_unique<MenuButton>("Quit"));
        in_game_menu_->SetPositionX(10);
        in_game_menu_->SetPositionY(10);
        in_game_menu_->SetWidth(200);
        in_game_menu_->SetHeight(200);
        render_engine_->AddWidget(in_game_menu_);
    }

    void InGameMenuController::CloseMenu(int scancode, int action, int mods)
    {
        if(action != GLFW_PRESS)
        {
            return;
        }

        render_engine_->RemoveWidget(in_game_menu_);
        input_stack_.Pop();
    }
}