/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin G?rka

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

#ifndef PLAINCRAFT_CORE_INPUT_EVENT
#define PLAINCRAFT_CORE_INPUT_EVENT

#include "../event.hpp"

namespace plaincraft_core {
	class InputEvent : public Event {
	private:
		char key_code_;
		int action_;

	public:
		InputEvent(char key_code, int action);
		InputEvent(const InputEvent& other);
		InputEvent(InputEvent&& other) noexcept;

		InputEvent& operator=(const InputEvent& other);
		InputEvent& operator=(InputEvent&& other) noexcept;

		inline EventType GetType() const override {
			return EventTypes::INPUT_EVENT;
		}

		const char Test() const {
			return 1;
		};

		auto GetKeyCode() const -> const char { return key_code_; }
		auto GetAction() const -> const int { return action_; }
	};
}
#endif // PLAINCRAFT_CORE_INPUT_EVENT