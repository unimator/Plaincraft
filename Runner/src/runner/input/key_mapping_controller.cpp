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

namespace plaincraft_runner {
    std::shared_ptr<KeyMappingController> KeyMappingController::CreateInstance() {
        return std::shared_ptr<KeyMappingController>(new KeyMappingController());
    }

    void KeyMappingController::Setup(Game& game_instance) {
        auto events_manager = game_instance.GetEventsManager();
        auto player = game_instance.GetScene().FindEntityByName("player");
        player_ = player;
        auto camera = game_instance.GetCamera();
        camera_ = camera;
        events_manager->Subscribe(EventType::INPUT_EVENT, shared_from_this());
        events_manager->Subscribe(EventType::LOOP_EVENT, shared_from_this());
    }

    void KeyMappingController::OnEventTriggered(const Event& event) {
        static rp3d::Vector3 target;
        static bool forward = false, backward = false, left = false, right = false; 

        switch(event.GetType()) {
            case EventType::INPUT_EVENT: 
            {
                const auto& input_event = static_cast<const InputEvent&>(event);
                const auto key_code = input_event.GetKeyCode();
                const auto action = input_event.GetAction();
                if(action == GLFW_PRESS || action == GLFW_RELEASE) 
                {
                    auto was_pressed = action == GLFW_PRESS;
                    if (key_code == GLFW_KEY_W)
                    {
                        forward = was_pressed; 
                    }
                    if (key_code == GLFW_KEY_S)
                    {
                        backward = was_pressed;
                    }
                    if (key_code == GLFW_KEY_A)
                    {
                        left = was_pressed;
                    }
                    if (key_code == GLFW_KEY_D)
                    {
                        right = was_pressed;
                    }
                    if (action == GLFW_PRESS && key_code == GLFW_KEY_SPACE)
                    {
                        if(abs(player_->GetRigidBody()->getLinearVelocity().y) < 0.00001)
                        { 
                            player_->GetRigidBody()->applyForceToCenterOfMass(rp3d::Vector3(0.0, 250.0, 0.0));
                        }
                    }
                }

                break;
            }
            case EventType::LOOP_EVENT:
            {
                
                if(forward || backward || left || right)
                {
                    auto loop_event = static_cast<const LoopEvent&>(event);
                    
                    const auto direction = camera_->direction;
                    target = rp3d::Vector3::zero();
                    
                    if(forward)
                    {
                        target += rp3d::Vector3(direction.x, 0, direction.z);
                    }
                    if(backward)
                    {
                        target -= rp3d::Vector3(direction.x, 0, direction.z);
                    }
                    if(right)
                    {
                        target += rp3d::Vector3(direction.x, 0, direction.z).cross(FromGlm(camera_->up));
                    }
                    if(left)
                    {
                        target -= rp3d::Vector3(direction.x, 0, direction.z).cross(FromGlm(camera_->up));
                    }

                    target.normalize();
                    target *= movement_speed_;

                    auto current_velocity = player_->GetRigidBody()->getLinearVelocity();
                    
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

                    target.y = current_velocity.y;
                    
                    player_->GetRigidBody()->setLinearVelocity(target);
                }

                break;
            }
        }


       
           
    }
}