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

#ifndef PLAINCRAFT_RENDER_ENGINE_OPENGL_OPENGL_SHADER
#define PLAINCRAFT_RENDER_ENGINE_OPENGL_OPENGL_SHADER
#include "../common.hpp"
#include <plaincraft_render_engine.hpp>

namespace plaincraft_render_engine_opengl {
	class OpenGLShader
	{
	private:
		uint32_t program_id_;

	public:
		OpenGLShader(const std::string& vertex_shader, const std::string& fragment_shader);
		virtual ~OpenGLShader();

		OpenGLShader(const OpenGLShader& other) = delete;
		OpenGLShader& operator=(const OpenGLShader& other) = delete;

		OpenGLShader(OpenGLShader&& other) noexcept;
		OpenGLShader& operator=(OpenGLShader&& other) noexcept;

		void Use();
		void SetModelViewProjection(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
		void SetMat4(const std::string& name, const glm::mat4& value);

	private:
		uint32_t CreateShader(const std::string& shader_source, uint32_t shader_type);
	};
}

#endif // PLAINCRAFT_RENDER_ENGINE_OPENGL_OPENGL_SHADER