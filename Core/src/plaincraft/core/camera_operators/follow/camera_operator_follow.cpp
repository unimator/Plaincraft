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

#include "camera_operator_follow.hpp"
#include "glm/glm.hpp"

namespace plaincraft_core {
    CameraOperatorFollow::CameraOperatorFollow(std::shared_ptr<Camera> camera, std::shared_ptr<GameObject> follow_target, float distance_to_target)
    : CameraOperator(camera), follow_target_(follow_target), distance_to_target_(distance_to_target) {}

    void CameraOperatorFollow::HandleCameraMovement(double delta_horiz, double delta_vert, double delta_time)
    {
        const float camera_movement_speed = 0.05f;
        const float camera_sensitivity = 0.25f;

        delta_horiz *= camera_sensitivity * delta_time;
        delta_vert *= camera_sensitivity * delta_time;

        camera_->yaw += delta_horiz;
        camera_->pitch -= delta_vert;

        if (camera_->pitch > (glm::half_pi<float>() - 0.05f)) {
            camera_->pitch = glm::half_pi<float>() - 0.05f;
        }

        if (camera_->pitch < -(glm::half_pi<float>() - 0.05f)) {
            camera_->pitch = -(glm::half_pi<float>() - 0.05f);
        }

        auto target_position = follow_target_->GetDrawable()->GetPosition();
        camera_->position = Vector3d(
            target_position.x + distance_to_target_ * glm::cos(camera_->pitch) * glm::cos(camera_->yaw),
            target_position.y + distance_to_target_ * glm::sin(camera_->pitch),
            target_position.z + distance_to_target_ * glm::cos(camera_->pitch) * glm::sin(camera_->yaw)
        );

        camera_->direction = target_position - camera_->position;

        // camera_->direction.x = static_cast<float>(cos(glm::radians(camera_->yaw)) * cos(glm::radians(camera_->pitch)));
        // camera_->direction.y = static_cast<float>(sin(glm::radians(camera_->pitch)));
        // camera_->direction.z = static_cast<float>(sin(glm::radians(camera_->yaw)) * cos(glm::radians(camera_->pitch)));
        // camera_->direction = glm::normalize(camera_->direction);

        // camera_->position = follow_target_->GetPosition();
    }
}