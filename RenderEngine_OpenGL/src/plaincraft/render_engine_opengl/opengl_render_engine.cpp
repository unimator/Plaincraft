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

#include "opengl_render_engine.hpp"
#include "shader\opengl_shader.hpp"
#include "renderer\opengl_renderer.hpp"
#include "texture\opengl_textures_factory.hpp"

namespace plaincraft_render_engine_opengl {
	OpenGLRenderEngine::OpenGLRenderEngine(uint32_t width, uint32_t height) : RenderEngine(width, height) {
		CreateWindow();
		renderer_ = std::make_shared<OpenGLRenderer>(OpenGLRenderer(camera_));
		textures_factory_ = std::make_shared<OpenGLTexturesFactory>(OpenGLTexturesFactory());
	}

	OpenGLRenderEngine::~OpenGLRenderEngine() {
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void OpenGLRenderEngine::CreateWindow() {
		glewExperimental = true;

		glfwInit();

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

		window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_FALSE);

		glfwMakeContextCurrent(window_);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glfwSwapInterval(0);
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			throw std::runtime_error("Failed to initialize GLEW");
		}

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRenderEngine::RenderFrame() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderEngine::RenderFrame();
		
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

	std::unique_ptr<plaincraft_render_engine::Shader> OpenGLRenderEngine::CreateDefaultShader() {
		auto vertex_shader = read_file("..\\..\\..\\Core\\resources\\vertexshader.shader"); // shaders should be copied to binary directory or something like this
		auto fragment_shader = read_file("..\\..\\..\\Core\\resources\\fragmentshader.shader"); // shaders should be copied to binary directory or something like this

		return std::make_unique<OpenGLShader>(std::move(OpenGLShader(vertex_shader, fragment_shader)));
	}
}