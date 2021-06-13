/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Górka

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

#include "box_collider.hpp"

namespace plaincraft_core {

	std::array<Plane, 6> initialize_bounding_planes(Vector3d position, Quaternion rotation, float width, float height, float depth)
	{

		/*
		    [f]--------------[g]
		    /|               /|
		   / |              / |
		  /  |             /  |
		[b]--------------[c]  |
		 |   |            |   |
		 |   |            |   |
		 |  [e]___________|__[h]
		 |  /             |  /
		 | /              | / 
		 |/               |/ 
		[a]--------------[d]
		
		*/


		auto a = Vector3d(-width / 2.0f, -height / 2.0, depth/2.0);
		auto b = Vector3d(-width / 2.0f, height / 2.0, depth / 2.0);
		auto c = Vector3d(width / 2.0f, height / 2.0, depth / 2.0);
		auto d = Vector3d(width / 2.0f, -height / 2.0, depth / 2.0);
		auto e = Vector3d(-width / 2.0f, -height / 2.0, -depth / 2.0);
		auto f = Vector3d(-width / 2.0f, height / 2.0, -depth / 2.0);
		auto g = Vector3d(width / 2.0f, height / 2.0,- depth / 2.0);
		auto h = Vector3d(width / 2.0f, -height / 2.0, -depth / 2.0);

		auto translate = glm::translate(glm::mat4(1.0f), position);

		auto translate_and_rotate = [rotation, translate](Vector3d& point) {
			point = rotation * point;
			auto position = glm::vec4(point, 1.0f);
			position = translate * position;
			point = Vector3d(position);
		};

		translate_and_rotate(a);
		translate_and_rotate(b);
		translate_and_rotate(c);
		translate_and_rotate(d);
		translate_and_rotate(e);
		translate_and_rotate(f);
		translate_and_rotate(g);
		translate_and_rotate(h);

		std::array<Plane, 6> planes = {
			Plane(0, 0, 0, 0),
			Plane(0, 0, 0, 0),
			Plane(0, 0, 0, 0),
			Plane(0, 0, 0, 0),
			Plane(0, 0, 0, 0),
			Plane(0, 0, 0, 0)
		};

		return std::move(planes);
	}
	
	BoxCollider::BoxCollider(Quaternion rotation, float width, float height, float depth) 
		: width_(width), height_(height), depth_(depth)
	{
		rotation_ = rotation;
	}

	BoxCollider::~BoxCollider() {}

	float BoxCollider::GetSphericalRadius() const {
		return sqrtf(width_ * width_ + height_ * height_ + depth_ * depth_);
	}
}