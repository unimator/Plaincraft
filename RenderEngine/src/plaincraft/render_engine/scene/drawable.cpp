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

#include "drawable.hpp"
#include <glm\gtx\quaternion.hpp>

namespace plaincraft_render_engine
{
	void Drawable::SetModel(std::shared_ptr<Model> model)
	{
		model_ = model;
	}

	std::shared_ptr<Model> Drawable::GetModel() const
	{
		return model_;
	}

	void Drawable::SetTexture(std::shared_ptr<Texture> texture)
	{
		texture_ = texture;
	}

	std::shared_ptr<Texture> Drawable::GetTexture() const
	{
		return texture_;
	}

	void Drawable::SetScale(float scale)
	{
		scale_ = scale;
		CalculateModelMatrix();
	}

	float Drawable::GetScale() const
	{
		return scale_;
	}

	void Drawable::SetRotation(Quaternion rotation)
	{
		rotation_ = rotation;
		CalculateModelMatrix();
	}

	Quaternion Drawable::GetRotation() const
	{
		return rotation_;
	}

	void Drawable::SetPosition(Vector3d position)
	{
		position_ = position;
		CalculateModelMatrix();
	}

	Vector3d Drawable::GetPosition() const 
	{
		return position_;
	}

	void Drawable::SetColor(Vector3d color)
	{
		color_ = color;
	}

	Vector3d Drawable::GetColor() const
	{
		return color_;
	}

	glm::mat4 Drawable::GetModelMatrix() const
	{
		return model_matrix_;
	}

	void Drawable::CalculateModelMatrix()
	{
		model_matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::scale(glm::mat4(1.0f), Vector3d(scale_, scale_, scale_)) * glm::toMat4(rotation_);
	}
}