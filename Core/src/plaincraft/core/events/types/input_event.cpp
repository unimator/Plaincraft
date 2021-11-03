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

#include "input_event.hpp"

namespace plaincraft_core {

	InputEvent::InputEvent(char key_code, int action) : key_code_(key_code), action_(action) {}

	InputEvent::InputEvent(const InputEvent& other)
	{
		key_code_ = other.key_code_;
		action_ = other.action_;
	}

	InputEvent::InputEvent(InputEvent&& other) noexcept
	{
		key_code_ = other.key_code_;
		action_ = other.action_;
		other.key_code_ = 0;
		other.action_ = 0;
	}

	InputEvent& InputEvent::operator=(const InputEvent& other)
	{
		key_code_ = other.key_code_;
		action_ = other.action_;

		return *this;
	}

	InputEvent& InputEvent::operator=(InputEvent&& other) noexcept
	{
		key_code_ = other.key_code_;
		action_ = other.action_;
		other.key_code_ = 0;
		other.action_ = 0;

		return *this;
	}

	char InputEvent::GetKeyCode() const {
		return key_code_;
	}

	int InputEvent::GetAction() const {
		return action_;
	}
}