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

#ifndef PLAINCRAFT_CORE_PLAYER
#define PLAINCRAFT_CORE_PLAYER

#include "../common.hpp"
#include "../events/event_observer.hpp"
#include "../events/types/input_event.hpp"
#include "entity.hpp"
#include <plaincraft_render_engine.hpp>

namespace plaincraft_core {
	using namespace plaincraft_render_engine;

	class Player : public Entity, public EventObserver {
	private:
		const float acceleration_ = 2.0f;
		const float max_speed_ = 10.0f;
		const float jump_power_ = 50.0f;
		float velocity_ = 0.0f;
		bool acceleration_flag_ = false;
		bool forward_ = false, back_ = false, left_ = false, right_ = false, jump_ = false;
		Vector3d velocity_vector_;
		std::shared_ptr<Camera> camera_;

	public:
		Player(std::shared_ptr<Camera> camera);
		Player(const Player& other) = delete;
		Player(Player&& other) noexcept;

		Player& operator=(const Player& other) = delete;
		Player& operator=(Player&& other) noexcept;

		virtual ~Player();

		//void Move(double delta_x, double delta_y);

		void OnEventTriggered(const Event& event) override;
	};
}

#endif // PLAINCRAFT_CORE_PLAYER