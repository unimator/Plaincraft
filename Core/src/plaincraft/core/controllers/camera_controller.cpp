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

#include "camera_controller.hpp"

namespace plaincraft_core
{
    CameraController::CameraController(std::shared_ptr<CameraOperator> camera_operator)
        : input_target_(InputTarget::TargetType::Passive, InputTarget::CursorVisibility::Hidden), camera_operator_(camera_operator)
    {
        input_target_.mouse_movement.AddSubscription(this, &CameraController::OnMouseMovement);
    }

    InputTarget& CameraController::GetInputTarget()
    {
        return input_target_;
    }

    void CameraController::OnMouseMovement(double delta_horiz, double delta_vert, float delta_time)
    {
        camera_operator_->HandleCameraMovement(delta_horiz, delta_vert, delta_time);
    }
}