/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin G�rka

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

#include "body.hpp"

namespace plaincraft_core {
	const float Body::minimum_velocity_ = 0.0001f;

	Body::Body() {
		velocity_ = Vector3d(0.0f, 0.0f, 0.0f);
	}

	void Body::SetCollider(std::shared_ptr<Collider> collider) {
		collider_ = std::move(collider);
	}

	std::shared_ptr<Collider> Body::GetCollider() const {
		return collider_;
	}

	void Body::SetPosition(Vector3d position) {
		position_ = std::move(position);
	}


	Vector3d Body::GetPosition() const {
		return position_;
	}

	void Body::SetVelocity(Vector3d velocity) {
		velocity_ = std::move(velocity);
		if (glm::length(velocity_) < minimum_velocity_) {
			velocity_ = Vector3d(0.0f, 0.0f, 0.0f);
		}
	}
	
	Vector3d Body::GetVelocity() const {
		return velocity_;
	}

	const float Body::GetFriction() const {
		return friction_factor_;
	}

	const bool Body::IsActive() const {
		return glm::length(velocity_) > minimum_velocity_;
	}

	void Body::AddForce(Vector3d force)
	{
		velocity_ += force / mass_;
	}
}