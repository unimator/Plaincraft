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

#include "opengl_window.hpp"
#include "../common.hpp"

namespace plaincraft_render_engine_opengl
{
	OpenGLWindow::OpenGLWindow(std::string title, uint32_t width, uint32_t height)
		: Window(title, width, height)
	{
		Initialize();
	}

	void OpenGLWindow::Initialize()
	{
		glewExperimental = true;

		glfwInit();

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

		instance_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
		glfwSetInputMode(instance_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(instance_, GLFW_STICKY_KEYS, GL_FALSE);

		glfwMakeContextCurrent(instance_);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glfwSwapInterval(0);
		if (glewInit() != GLEW_OK)
		{
			fprintf(stderr, "Failed to initialize GLEW\n");
			throw std::runtime_error("Failed to initialize GLEW");
		}

		glEnable(GL_DEPTH_TEST);

		window_events_handler_ = std::make_shared<WindowEventsHandler>(instance_);
	}
}