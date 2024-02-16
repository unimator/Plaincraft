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

#ifndef PLAINCRAFT_CORE_INPUT_TARGET
#define PLAINCRAFT_CORE_INPUT_TARGET

#include <plaincraft_common.hpp>
#include "input_stack.hpp"
#include <functional>
#include <unordered_map>

namespace plaincraft_core
{
    using namespace plaincraft_common;

    class InputTarget
    {
    public:
        enum TargetType
        {
            Passive,
            SemiBlocking,
            Blocking
        } target_type_;

        enum CursorVisibility
        {
            Visible,
            Hidden
        } cursor_visibility_;

        using KeyPressedEventTrigger = EventTrigger<int, int, int>;
        using MouseMovementTrigger = EventTrigger<double, double, float>;

    public:
        InputTarget(TargetType type, CursorVisibility cursor_visibility);

        std::unordered_map<int, KeyPressedEventTrigger> key_mappings;
        MouseMovementTrigger mouse_movement;
        EventTrigger<InputStack::StackEventType> on_input_stack_change;

        void SetTargetType(TargetType target_type);
        TargetType GetTargetType() const;

        void SetCursorVisibility(CursorVisibility cursur_visibility);
        CursorVisibility GetCursorVisibility() const;
    };
}

#endif // PLAINCRAFT_CORE_INPUT_TARGET