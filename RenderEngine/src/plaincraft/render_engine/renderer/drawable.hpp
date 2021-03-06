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

#ifndef PLAINCRAFT_RENDER_ENGINE_DRAWABLE
#define PLAINCRAFT_RENDER_ENGINE_DRAWABLE
#include "../common.hpp"
#include "model.hpp"
#include "../shader/shader.hpp"

namespace plaincraft_render_engine
{
	class DLLEXPORT_PLAINCRAFT_RENDER_ENGINE Drawable
	{
	private:
		std::shared_ptr<Model> model_;
		std::shared_ptr<Shader> shader_;
		Vector3d position_;
		Quaternion rotation_;
		float scale_ = 1.0f;


	public:
		void SetModel(std::shared_ptr<Model> model);
		std::shared_ptr<Model> GetModel() const;

		void SetShader(std::shared_ptr<Shader> shader);
		std::shared_ptr<Shader> GetShader() const;

		auto SetScale(float scale) -> void { scale_ = scale; }
		auto GetScale() const -> const float { return scale_; }

		auto GetRotation() -> Quaternion { return rotation_; }

		void SetPosition(Vector3d position);
		auto GetPosition() -> Vector3d { return position_; }
	};
}


#endif // PLAINCRAFT_RENDER_ENGINE_DRAWABLE