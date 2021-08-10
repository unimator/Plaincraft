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

#include "opengl_shader.hpp"
#include <iostream>

namespace plaincraft_render_engine_opengl {
	OpenGLShader::OpenGLShader(const std::string& vertex_shader, const std::string& fragment_shader)
	{
		int success;
		char infoLog[512];

		auto vertex_shader_id = CreateShader(vertex_shader, GL_VERTEX_SHADER);
		auto fragment_shader_id = CreateShader(fragment_shader, GL_FRAGMENT_SHADER);

		program_id_ = glCreateProgram();

		glAttachShader(program_id_, vertex_shader_id);
		glAttachShader(program_id_, fragment_shader_id);
		glLinkProgram(program_id_);

		glGetShaderiv(program_id_, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(program_id_, 512, NULL, infoLog);
			std::cout << "COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
	}

	OpenGLShader::OpenGLShader(OpenGLShader&& other) noexcept {
		program_id_ = other.program_id_;
		other.program_id_ = -1;
	}

	OpenGLShader& OpenGLShader::operator=(OpenGLShader&& other) noexcept {
		program_id_ = other.program_id_;
		other.program_id_ = -1;
		return *this;
	}

	OpenGLShader::~OpenGLShader() {

	}

	uint32_t OpenGLShader::CreateShader(const std::string& shader_source, uint32_t shader_type)
	{
		int success;
		char infoLog[512];

		auto shader_id = glCreateShader(shader_type);
		const auto shader_source_str = shader_source.c_str();

		glShaderSource(shader_id, 1, &shader_source_str, nullptr);
		glCompileShader(shader_id);

		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
		if (!GL_TRUE)
		{
			glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
			std::cout << "COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		return shader_id;
	}

	void OpenGLShader::Use() {
		glUseProgram(program_id_);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
		glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}

	void OpenGLShader::SetModelViewProjection(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
		SetMat4("model", model);
		SetMat4("view", view);
		SetMat4("projection", projection);
	}
}