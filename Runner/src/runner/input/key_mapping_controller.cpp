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

#include "key_mapping_controller.hpp"
#include <cstdio>

using namespace plaincraft_core;

namespace plaincraft_runner
{
    KeyMappingController::KeyMappingController(Game& game_instance)
    :   game_instance_(game_instance)
    {
        key_mappings_.insert(std::make_pair(GLFW_KEY_W, &KeyMappingController::MoveForward));
        key_mappings_.insert(std::make_pair(GLFW_KEY_S, &KeyMappingController::MoveBackward));
        key_mappings_.insert(std::make_pair(GLFW_KEY_A, &KeyMappingController::MoveLeft));
        key_mappings_.insert(std::make_pair(GLFW_KEY_D, &KeyMappingController::MoveRight));
        key_mappings_.insert(std::make_pair(GLFW_KEY_SPACE, &KeyMappingController::Jump));
        key_mappings_.insert(std::make_pair(GLFW_KEY_F1, &KeyMappingController::ToggleDebugInfo));
    }

    std::shared_ptr<KeyMappingController> KeyMappingController::CreateInstance(Game& game_instance)
    {
        return std::shared_ptr<KeyMappingController>(new KeyMappingController(game_instance));
    }

    void KeyMappingController::Setup()
    {
        auto player = game_instance_.GetScene().FindGameObjectByName("player");
        player_ = player;
        auto camera = game_instance_.GetCamera();
        camera_ = camera;
        
        auto& window_events_handler = game_instance_.GetWindowEventsHandler();
        auto& loop_events_handler = game_instance_.GetLoopEventsHandler();
        window_events_handler.key_pressed_event_trigger.AddSubscription(this, &KeyMappingController::OnKeyPressed);
        loop_events_handler.loop_event_trigger.AddSubscription(this, &KeyMappingController::OnLoopTick);
    }

    

    void KeyMappingController::MoveForward(int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            forward_ = was_pressed;
        }
    }

    void KeyMappingController::MoveBackward(int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            backward_ = was_pressed;
        }
    }

    void KeyMappingController::MoveLeft(int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            left_ = was_pressed;
        }
    }

    void KeyMappingController::MoveRight(int action, int mods)
    {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
            auto was_pressed = action == GLFW_PRESS;
            right_ = was_pressed;
        }
    }

    void KeyMappingController::Jump(int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            // if (abs(player_->GetRigidBody()->getLinearVelocity().y) < 0.0001)
            // {
            //     player_->GetRigidBody()->applyLocalForceAtCenterOfMass(rp3d::Vector3(0.0, 300.0, 0.0));
            // }
        }
    }

    void KeyMappingController::ToggleDebugInfo(int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            auto& global_state = game_instance_.GetGlobalState();
            auto is_debug_info_visible = global_state.GetDebugInfoVisibility();
            global_state.SetDebugInfoVisibility(!is_debug_info_visible);
        }
    }

    void KeyMappingController::OnKeyPressed(int key, int scancode, int action, int mods)
    {
        if(key_mappings_.contains(key))
        {
            auto& action_callback = key_mappings_[key];
            (this->*action_callback)(action, mods);
        }
    }

    void KeyMappingController::OnLoopTick(float delta_time)
    {
        // static rp3d::Vector3 target;

        // if (forward_ || backward_ || left_ || right_)
        //     {
        //         const auto direction = camera_->direction;
        //         target = rp3d::Vector3::zero();

        //         if (forward_)
        //         {
        //             target += rp3d::Vector3(direction.x, 0, direction.z);
        //         }
        //         if (backward_)
        //         {
        //             target -= rp3d::Vector3(direction.x, 0, direction.z);
        //         }
        //         if (right_)
        //         {
        //             target += rp3d::Vector3(direction.x, 0, direction.z).cross(FromGlm(camera_->up));
        //         }
        //         if (left_)
        //         {
        //             target -= rp3d::Vector3(direction.x, 0, direction.z).cross(FromGlm(camera_->up));
        //         }

        //         target.normalize();
        //         target *= movement_speed_;

        //         auto current_velocity = player_->GetRigidBody()->getLinearVelocity();

                /*if(abs(target.x + current_velocity.x) < maximum_speed_) {
                        target.x += current_velocity.x;
                    } else if (current_velocity.x < maximum_speed_) {
                        target.x = maximum_speed_;
                    } else if (current_velocity.x > -maximum_speed_) {
                        target.x = -maximum_speed_;
                    } else {
                        target.x = current_velocity.x;
                    }

                    if(abs(target.y + current_velocity.y) < maximum_speed_) {
                        target.y += current_velocity.y;
                    } else if (current_velocity.y < maximum_speed_) {
                        target.y = maximum_speed_;
                    } else if (current_velocity.y > -maximum_speed_) {
                        target.y = -maximum_speed_;
                    } else {
                        target.y = current_velocity.y;
                    }*/

                // target.y = current_velocity.y;

                // player_->GetRigidBody()->setLinearVelocity(target);
            // }
    }
}