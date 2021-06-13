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

#ifndef PLAINCRAFT_CORE_COLLISIONS_DETECTOR
#define PLAINCRAFT_CORE_COLLISIONS_DETECTOR
#include "../../common.hpp"
#include "../collider/collider.hpp"
#include "../collider/box_collider.hpp"

namespace plaincraft_core {
	class CollisionsDetector
	{
	public:
		bool Detect(
			const std::shared_ptr<Collider>& first, const Vector3d& first_position, const Vector3d& first_translation,
			const std::shared_ptr<Collider>& second, const Vector3d& second_position, const Vector3d& second_translation) const;

		Vector3d MinimumDistance(
			const std::shared_ptr<Collider>& first, const Vector3d& first_position,
			const std::shared_ptr<Collider>& second, const Vector3d& second_position,
			const Vector3d& translation_vector);

		Vector3d debug_point;
		Vector3d debug_vector;

	private:
		bool ObjectedBoundingBoxSeparatingAxisTheorm(
			const std::shared_ptr<BoxCollider>& first, const Vector3d& first_position, const Vector3d& first_translation,
			const std::shared_ptr<BoxCollider>& second, const Vector3d& second_position, const Vector3d& second_translation) const;

		Vector3d MinimumDistance(
			const std::shared_ptr<BoxCollider>& first, const Vector3d& first_position,
			const std::shared_ptr<BoxCollider>& second, const Vector3d& second_position,
			const Vector3d& translation_vector);
	};
}



#endif // PLAINCRAFT_CORE_COLLISIONS_DETECTOR