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

#include "entity_input_controller.hpp"
#include <functional>

namespace plaincraft_core
{
    EntityInputController::EntityInputController(std::shared_ptr<GameObject> target_entity, std::shared_ptr<Camera> camera)
        : input_target_(InputTarget::TargetType::Blocking), target_entity_(target_entity), camera_(camera)
    {
        input_target_.key_mappings[GLFW_KEY_W] = std::bind(&EntityInputController::MoveForward, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        input_target_.key_mappings[GLFW_KEY_S] = std::bind(&EntityInputController::MoveBackward, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        input_target_.key_mappings[GLFW_KEY_A] = std::bind(&EntityInputController::MoveLeft, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        input_target_.key_mappings[GLFW_KEY_D] = std::bind(&EntityInputController::MoveRight, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        input_target_.key_mappings[GLFW_KEY_SPACE] = std::bind(&EntityInputController::Jump, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        input_target_.key_mappings[GLFW_KEY_LEFT_CONTROL] = std::bind(&EntityInputController::Crouch, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    InputTarget &EntityInputController::GetInputTarget()
    {
        return input_target_;
    }

    void EntityInputController::MoveForward(int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            forward_ = was_pressed;
        }
    }

    void EntityInputController::MoveBackward(int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            backward_ = was_pressed;
        }
    }

    void EntityInputController::MoveLeft(int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            left_ = was_pressed;
        }
    }

    void EntityInputController::MoveRight(int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            right_ = was_pressed;
        }
    }

    void EntityInputController::Jump(int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            jump_ = was_pressed;
        }
    }

    void EntityInputController::Crouch(int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            crouch_ = was_pressed;
        }
    }

    void EntityInputController::OnLoopTick(float delta_time)
    {
        if (forward_ || backward_ || left_ || right_ || jump_ || crouch_)
        {
            const auto direction = camera_->direction;
            const auto &physics_object = target_entity_->GetPhysicsObject();
            Vector3d target{};

            if (forward_)
            {
                target += Vector3d(direction.x, 0, direction.z);
            }
            if (backward_)
            {
                target -= Vector3d(direction.x, 0, direction.z);
            }
            if (right_)
            {
                target += glm::cross(Vector3d(direction.x, 0, direction.z), camera_->up);
            }
            if (left_)
            {
                target -= glm::cross(Vector3d(direction.x, 0, direction.z), camera_->up);
            }
            if (glm::length(target) > 0.000001f)
            {
                target = glm::normalize(target);
                target *= movement_speed_ * delta_time;
            }

            if (jump_ && physics_object->is_grounded)
            {
                target += Vector3d(0, 6.0f, 0);
                physics_object->is_grounded = false;
            }
            if (crouch_)
            {
                target += Vector3d(0, -1.0f, 0);
            }

            if (glm::length(target) <= 0.000001f)
            {
                return;
            }

            auto current_velocity = target_entity_->GetPhysicsObject()->velocity;
            target_entity_->GetPhysicsObject()->velocity += target;
        }
    }
}