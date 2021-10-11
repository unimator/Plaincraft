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

#include "player.hpp"
#include <iostream>
#include "../events/types/loop_event.hpp"
#include <plaincraft_render_engine_opengl.hpp>

namespace plaincraft_core {
	Player::Player(std::shared_ptr<Camera> camera) : camera_(camera), velocity_vector_(Vector3d(0, 0, 0)) {
		this->SetDrawable(nullptr);
	}

	Player::Player(Player&& other) noexcept
	{
		//position_ = std::move(other.position_);
		camera_ = std::move(other.camera_);
		velocity_vector_ = std::move(other.velocity_vector_);
	}

	Player& Player::operator=(Player&& other) noexcept {

		//position_ = std::move(other.position_);
		camera_ = std::move(other.camera_);
		velocity_vector_ = std::move(other.velocity_vector_);

		return *this;
	}

	Player::~Player() = default;

	void Player::OnEventTriggered(const Event& event)
	{
		const auto event_type = event.GetType();

		switch (event_type) {
		case EventTypes::INPUT_EVENT: {
			const InputEvent& input_event = static_cast<const InputEvent&>(event);
			auto key_code = input_event.GetKeyCode();
			const auto action = input_event.GetAction();

			if (action == GLFW_PRESS || action == GLFW_RELEASE) {
				acceleration_flag_ = action == GLFW_PRESS;
				
				if (key_code == GLFW_KEY_W)
				{
					forward_ = acceleration_flag_;
				}
				if (key_code == GLFW_KEY_S)
				{
					back_ = acceleration_flag_;
				}
				if (key_code == GLFW_KEY_A)
				{
					left_ = acceleration_flag_;
				}
				if (key_code == GLFW_KEY_D)
				{
					right_ = acceleration_flag_;
				}
				if (key_code == GLFW_KEY_SPACE) 
				{
					jump_ = acceleration_flag_;
				}
			}
			break;
		}
		case EventTypes::LOOP_EVENT: {
			const LoopEvent& loop_event = static_cast<const LoopEvent&>(event);
			const auto delta_time = loop_event.GetDeltaTime();
			const static Vector3d horizontal_movement_vector(1.0f, 0.0f, 1.0f);
			const static Vector3d vertical_movement_vector(0.0f, 1.0f, 0.0f);
			Vector3d acceleration_vector(0.0f, 0.0f, 0.0f);

			if (forward_) {
				acceleration_vector = horizontal_movement_vector * acceleration_ * normalize(camera_->direction) * delta_time;
			}
			
			if (back_) {
				acceleration_vector = -horizontal_movement_vector * acceleration_ * normalize(camera_->direction) * delta_time;
			}
			
			if (left_) {
				acceleration_vector = -horizontal_movement_vector * acceleration_ * normalize(cross(camera_->direction, camera_->up)) * delta_time;
			}
			
			if (right_) {
				acceleration_vector = horizontal_movement_vector * acceleration_ * normalize(cross(camera_->direction, camera_->up)) * delta_time;
			}

			if(jump_) {
				if(GetBody()->GetVelocity().y <= 0.0001f)
				{
					acceleration_vector += vertical_movement_vector * jump_power_ * delta_time;
				}
			}

			/*const auto player_position = GetPosition();
			SetPosition(player_position + velocity_vector_ * delta_time);
			camera_->position.x = player_position.x;
			camera_->position.z = player_position.z;
			break;*/

			camera_->position = GetBody()->GetPosition();
			camera_->position.y = camera_->position.y + 1.5f;

			acceleration_vector.y -= 1.0f * delta_time;
			GetBody()->AddForce(acceleration_vector);

			GetDrawable()->SetPosition(GetBody()->GetPosition());

			break;
		}
		}
	}
}