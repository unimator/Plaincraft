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

#ifndef PLAINCRAFT_CORE_COLLIDER
#define PLAINCRAFT_CORE_COLLIDER

#include "../../common.hpp"

namespace plaincraft_core {
	class Collider {

	protected:
		Quaternion rotation_;

	public:
		Collider() : rotation_(Quaternion(1.0f, 0.0f, 0.0f, 0.0f)) {};

		virtual ~Collider() {};

		virtual float GetSphericalRadius() const = 0;

		Quaternion GetRotation() { return rotation_; }
		
		void SetRotation(Quaternion rotation) {
			rotation_ = std::move(rotation);
		}
	};
}
#endif // PLAINCRAFT_CORE_COLLIDER