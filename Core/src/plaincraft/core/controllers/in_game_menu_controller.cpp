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
        GlobalState &global_state,
        Cache<Font> &fonts_cache,
        InputStack &input_stack)
        : input_target_(InputTarget::TargetType::Passive, InputTarget::CursorVisibility::Hidden),
          menu_input_target_(InputTarget::TargetType::Blocking, InputTarget::CursorVisibility::Visible),
          render_engine_(render_engine),
          global_state_(global_state),
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

        auto window = render_engine_->GetWindow();
        auto width = window->GetWidth();
        auto height = window->GetHeight();

        auto &font_utils = render_engine_->GetFontsFactory()->GetFontUtils();

        auto standard_font = fonts_cache_.Fetch("standard");
        auto button_height = font_utils.CalcStringHeight("Quit", standard_font);

        auto &menu_factory = render_engine_->GetMenuFactory();

        auto quit_button = std::make_unique<MenuButton>("Quit");
        quit_button->on_button_click.AddSubscription(this, &InGameMenuController::QuitCallback);

        auto resume_button = std::make_unique<MenuButton>("Resume");
        resume_button->on_button_click.AddSubscription(this, &InGameMenuController::ResumeCallback);

        in_game_menu_ = menu_factory->CreateMenu();
        in_game_menu_->SetFont(fonts_cache_.Fetch("standard"));
        in_game_menu_->AddButton(std::move(resume_button));
        in_game_menu_->AddButton(std::move(quit_button));
        in_game_menu_->SetPositionX(10);
        in_game_menu_->SetPositionY(height - 2 * button_height - 30);
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

    void InGameMenuController::QuitCallback()
    {
        global_state_.SetIsRunning(false);
    }

    void InGameMenuController::ResumeCallback()
    {
        render_engine_->RemoveWidget(in_game_menu_);
        input_stack_.Pop();
    }
}