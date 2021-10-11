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

#include "opengl_renderer.hpp"
#include <glm\gtx\quaternion.hpp>

namespace plaincraft_render_engine_opengl {
	OpenGLRenderer::OpenGLRenderer(std::shared_ptr<Camera> camera) : Renderer(camera) {
		glGenBuffers(1, &vbo_);
		glGenBuffers(1, &ebo_);
		glGenVertexArrays(1, &vao_);
		shader_ = std::make_unique<OpenGLShader>(CreateDefaultShader());
	}

	void OpenGLRenderer::Render() {
		auto drawable = drawables_list_.front();
		auto modelObject = drawable->GetModel();
		auto polygon = modelObject->GetMesh();
		const auto scale = drawable->GetScale();
		const auto position = drawable->GetPosition();
		const auto rotation = drawable->GetRotation();
		//auto currentFrame = glfwGetTime();

		glm::mat4 projection = glm::perspective(glm::radians(camera_->fov), (float)1024 / (float)768, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);
		auto model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), Vector3d(scale, scale, scale)) * glm::toMat4(rotation);
		//glm::mat4 model = glm::translate(glm::mat4(1.0f), entity->GetPosition());
		//model = glm::toMat4(entity->GetBody()->GetCollider()->GetRotation()) * model;

		shader_->Use();
		shader_->SetModelViewProjection(model, view, projection);

		glBindVertexArray(vao_);

		modelObject->GetTexture()->UseTexture();

		auto t = polygon->GetVertices().data();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * polygon->GetVertices().size(), polygon->GetVertices().data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * polygon->GetIndices().size(), polygon->GetIndices().data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr); // position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(float))); // color
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(float))); // text coord
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(polygon->GetIndices().size()), GL_UNSIGNED_INT, nullptr);

		drawables_list_.clear();
	}

	OpenGLShader OpenGLRenderer::CreateDefaultShader() 
	{
		auto vertex_shader = read_file("..\\..\\..\\Core\\resources\\vertexshader.shader"); // shaders should be copied to binary directory or something like this
		auto fragment_shader = read_file("..\\..\\..\\Core\\resources\\fragmentshader.shader"); // shaders should be copied to binary directory or something like this

		return OpenGLShader(vertex_shader, fragment_shader);
	}
}