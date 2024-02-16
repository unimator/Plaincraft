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

#include "input_target.hpp"
#include "input_stack.hpp"
#include <stack>

namespace plaincraft_core
{
    InputStack::InputStack(std::shared_ptr<RenderEngine> render_engine) : render_engine_(render_engine) {}

    void InputStack::Push(InputTarget &input_target)
    {
        input_targets_.push_back(std::ref(input_target));
        for (auto &input_target : input_targets_)
        {
            input_target.get().on_input_stack_change.Trigger(StackEventType::Push);
        }
        Apply(input_target);
    }

    void InputStack::Pop()
    {
        for (auto &input_target : input_targets_)
        {
            input_target.get().on_input_stack_change.Trigger(StackEventType::Pop);
        }
        input_targets_.pop_back();
        if (input_targets_.size() > 0)
        {
            Apply(input_targets_.back());
        }
    }

    void InputStack::SingleClickHandler(int key, int scancode, int action, int mods)
    {
        if (input_targets_.empty())
        {
            return;
        }

        auto key_callbacks = std::stack<std::reference_wrapper<InputTarget::KeyPressedEventTrigger>>();

        for (std::vector<std::reference_wrapper<InputTarget>>::reverse_iterator input_target_it = input_targets_.rbegin();
             input_target_it != input_targets_.rend();
             ++input_target_it)
        {
            auto &input_target = input_target_it->get();
            auto &key_mappings = input_target.key_mappings;

            if (key_mappings.contains(key))
            {
                auto &action_callback = key_mappings[key];
                key_callbacks.push(action_callback);

                if (input_target.GetTargetType() == InputTarget::TargetType::SemiBlocking)
                {
                    break;
                }
            }

            if (input_target.GetTargetType() == InputTarget::TargetType::Blocking)
            {
                break;
            }
        }

        while (!key_callbacks.empty())
        {
            auto &callback = key_callbacks.top();
            callback.get().Trigger(scancode, action, mods);
            key_callbacks.pop();
        }
    }

    void InputStack::MouseMovement(double delta_horiz, double delta_vert, float delta_time)
    {
        if (input_targets_.empty())
        {
            return;
        }

        auto mouse_callbacks = std::stack<std::reference_wrapper<InputTarget::MouseMovementTrigger>>();

        for (std::vector<std::reference_wrapper<InputTarget>>::reverse_iterator input_target_it = input_targets_.rbegin();
             input_target_it != input_targets_.rend();
             ++input_target_it)
        {
            auto &input_target = input_target_it->get();
            auto &mouse_movement = input_target.mouse_movement;

            mouse_movement.Trigger(delta_horiz, delta_vert, delta_time);

            if (input_target.GetTargetType() == InputTarget::TargetType::Blocking)
            {
                break;
            }
        }
    }

    void InputStack::Apply(InputTarget &input_target)
    {
        auto &window = *(render_engine_->GetWindow().get());
        window.SetCursorVisible(input_target.GetCursorVisibility() == InputTarget::CursorVisibility::Visible);
    }
}