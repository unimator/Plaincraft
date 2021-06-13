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

#include "entity.hpp"
#include "../physics/collider/void_collider.hpp"

namespace plaincraft_core {
	Entity::Entity() : drawable_(nullptr)  {

	}

	void Entity::SetPosition(Vector3d position) {
		body_->SetPosition(position);
		drawable_->SetPosition(position);
	}

	Vector3d Entity::GetPosition() {
		return body_->GetPosition();
	}

	void Entity::SetDrawable(std::shared_ptr<plaincraft_render_engine::Drawable> drawable) {
		drawable_ = std::move(drawable);
	}

	std::shared_ptr<plaincraft_render_engine::Drawable> Entity::GetDrawable() const {
		return drawable_;
	}

	void Entity::SetBody(std::shared_ptr<Body> body) {
		body_ = std::move(body);
	}

	std::shared_ptr<Body> Entity::GetBody() const {
		return body_;
	}
}