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

#include "physics_engine.hpp"
#include "../events/types/loop_event.hpp"
#include <plaincraft_render_engine.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace plaincraft_core {
	void PhysicsEngine::AddBody(std::shared_ptr<Body> body) {
		bodies_.push_back(std::move(body));
	}

	void PhysicsEngine::OnEventTriggered(const Event& event) {
		const auto event_type = event.GetType();

		switch (event_type) {
		case EventTypes::LOOP_EVENT: {
			/*const LoopEvent& loop_event = static_cast<const LoopEvent&>(event);
			const auto delta_time = loop_event.GetDeltaTime();
			const static Vector3d horizontal_movement_vector(1.0f, 0.0f, 1.0f);

			for (auto body : bodies_) {
				if (!body->IsActive()) {
					continue;
				}

				const auto body_collider = body->GetCollider();
				const auto body_position = body->GetPosition();
				auto body_translation_vector = body->GetVelocity() * delta_time;

				for (auto other : bodies_) {
					if (other == body) {
						continue;
					}

					const auto other_collider = other->GetCollider();
					const auto other_position = other->GetPosition();
					const auto other_translation_vector = other->GetVelocity() * delta_time;

					if (!collisions_detector_.Detect(body_collider, body_position, body_translation_vector, other_collider, other_position, other_translation_vector)) {
						
						const auto translation_vector = collisions_detector_.MinimalDistance(body_collider, body_position, other_collider, other_position, body_translation_vector);
						body_translation_vector = translation_vector;
						body->SetVelocity(translation_vector / delta_time);
					}
				}

				body->SetPosition(body->GetPosition() + body_translation_vector);
				body->SetVelocity(body->GetVelocity() - horizontal_movement_vector * body->GetVelocity() * body->GetFriction() * delta_time);
			}*/

			break;
		}
		}
	}
}