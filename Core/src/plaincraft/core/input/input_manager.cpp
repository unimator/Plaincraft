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

#include "input_manager.hpp"
#include "../events/types/input_event.hpp"
#include "../game.hpp"
#include <cstdio>
#include <iostream>

namespace plaincraft_core {

	InputManager::InputManager(std::shared_ptr<EventsManager> events_manager, GLFWwindow* window) : events_manager_(events_manager), window_(window) 
	{
		glfwSetKeyCallback(window, InputManager::ProcessInputWrapper);
	}

	void InputManager::ProcessInputWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
		game->input_manager_.ProcessInput(window, key, scancode, action, mods);
	}

	void InputManager::ProcessInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto event = InputEvent(key, action);
		events_manager_->Trigger(event);
	}

}