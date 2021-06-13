/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Górka

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

#include "camera.hpp"

namespace plaincraft_render_engine {
    void Camera::HandleCameraMovement(double delta_x, double delta_y, double delta_time) {
        const float camera_movement_speed = 0.05f;
        const float camera_sensitivity = 0.1f;

        delta_x *= camera_sensitivity;
        delta_y *= camera_sensitivity;

        yaw += delta_x;
        pitch += delta_y;

        if (pitch > 89.0f) {
            pitch = 89.0f;
        }

        if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        direction.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
        direction.y = static_cast<float>(sin(glm::radians(pitch)));
        direction.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
        direction = glm::normalize(direction);
    }
}