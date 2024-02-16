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

#ifndef PLAINCRAFT_CORE_ENTITY_INPUT_CONTROLLER
#define PLAINCRAFT_CORE_ENTITY_INPUT_CONTROLLER

#include "../entities/game_object.hpp"
#include "../input/input_target.hpp"
#include "../input/input_stack.hpp"
#include <plaincraft_render_engine.hpp>
#include <memory>

namespace plaincraft_core
{
    class EntityInputController
    {
    private:
        InputTarget input_target_;

        std::shared_ptr<GameObject> target_entity_;
        std::shared_ptr<Camera> camera_;

        float air_movement_speed_ = 0.125f * 50.0f;
        float ground_movement_speed_ = 0.125f * 500.0f;
        float maximum_speed_ = 3.0f;

        bool forward_ = false, backward_ = false, left_ = false, right_ = false, jump_ = false, crouch_ = false;

    public:
        EntityInputController(std::shared_ptr<GameObject> target_entity, std::shared_ptr<Camera> camera);

        InputTarget& GetInputTarget();

        void OnLoopTick(float delta_time);

        void MoveForward(int scancode, int action, int mods);
        void MoveBackward(int scancode, int action, int mods);
        void MoveLeft(int scancode, int action, int mods);
        void MoveRight(int scancode, int action, int mods);
        void Jump(int scancode, int action, int mods);
        void Crouch(int scancode, int action, int mods);
        void InputStateChanged(InputStack::StackEventType stack_event_type);
    };
}

#endif // PLAINCRAFT_CORE_ENTITY_INPUT_CONTROLLER