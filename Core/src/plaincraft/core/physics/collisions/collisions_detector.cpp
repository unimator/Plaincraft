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

#include "collisions_detector.hpp"
#include <glm/gtx/quaternion.hpp>
#include "../../common.hpp"

namespace plaincraft_core {
	bool CollisionsDetector::Detect(
		const std::shared_ptr<Collider>& first, const Vector3d& first_position, const Vector3d& first_translation,
		const std::shared_ptr<Collider>& second, const Vector3d& second_position, const Vector3d& second_translation) const 
	{
		const auto first_box = std::dynamic_pointer_cast<BoxCollider>(first);
		const auto second_box = std::dynamic_pointer_cast<BoxCollider>(second);
		if (first_box != nullptr && second_box != nullptr) {
			return ObjectedBoundingBoxSeparatingAxisTheorm(first_box, first_position, first_translation, second_box, second_position, second_translation);
		}

		return false;
	};

	bool CollisionsDetector::ObjectedBoundingBoxSeparatingAxisTheorm(
		const std::shared_ptr<BoxCollider>& first, const Vector3d& first_position, const Vector3d& first_translation,
		const std::shared_ptr<BoxCollider>& second, const Vector3d& second_position, const Vector3d& second_translation) const 
	{
		auto ab_vector = (second_position + second_translation) - (first_position + first_translation);

		const auto a_x = glm::rotate(first->rotation_, Vector3d(1.0f, 0.0f, 0.0f));
		const auto a_y = glm::rotate(first->rotation_, Vector3d(0.0f, 1.0f, 0.0f));
		const auto a_z = glm::rotate(first->rotation_, Vector3d(0.0f, 0.0f, 1.0f));

		const auto a_width = first->width_;
		const auto a_height = first->height_;
		const auto a_depth = first->depth_;

		const auto b_x = glm::rotate(second->rotation_, Vector3d(1.0f, 0.0f, 0.0f));
		const auto b_y = glm::rotate(second->rotation_, Vector3d(0.0f, 1.0f, 0.0f));
		const auto b_z = glm::rotate(second->rotation_, Vector3d(0.0f, 0.0f, 1.0f));

		const auto b_width = second->width_;
		const auto b_height = second->height_;
		const auto b_depth = second->depth_;

		auto determine_face = [&ab_vector, &first, &second](
			const Vector3d& l,
			const float& dim_1,
			const float& dim_2_a, const float& dim_2_b, const float& dim_2_c,
			const Vector3d& vec_2_a, const Vector3d& vec_2_b, const Vector3d& vec_2_c) -> float {
				return -(dim_1
					+ glm::abs(dim_2_a * glm::dot(l, vec_2_a))
					+ glm::abs(dim_2_b * glm::dot(l, vec_2_b))
					+ glm::abs(dim_2_c * glm::dot(l, vec_2_c))
					- glm::abs(glm::dot(ab_vector, l)));
		};

		auto determine_edge = [&ab_vector](
			const Vector3d& a, const Vector3d& b, 
			const float& dim_a_1, const float& dim_a_2,
			const float& dim_b_1, const float& dim_b_2,
			const Vector3d& vec_a_1, const Vector3d& vec_a_2, 
			const Vector3d& vec_b_1, const Vector3d& vec_b_2
			) -> float {
				return -(glm::abs(dim_a_1 * glm::dot(vec_a_1, b))
					+ glm::abs(dim_a_2 * glm::dot(vec_a_2, b))
					+ glm::abs(dim_b_1 * glm::dot(a, vec_b_1))
					+ glm::abs(dim_b_2 * glm::dot(a, vec_b_2))
					- glm::abs(glm::dot(ab_vector, glm::cross(a, b))));
		};

		if (   determine_face(a_x, a_width, b_width, b_height, b_depth, b_x, b_y, b_z) > 0
			|| determine_face(a_y, a_height, b_width, b_height, b_depth, b_x, b_y, b_z) > 0
			|| determine_face(a_z, a_depth, b_width, b_height, b_depth, b_x, b_y, b_z) > 0
			
			|| determine_face(b_x, b_width, a_width, a_height, a_depth, a_x, a_y, a_z) > 0
			|| determine_face(b_y, b_height, a_width, a_height, a_depth, a_x, a_y, a_z) > 0
			|| determine_face(b_z, b_depth, a_width, a_height, a_depth, a_x, a_y, a_z) > 0

			|| determine_edge(a_x, b_x, a_height, a_depth, b_height, b_depth, a_z, a_y, b_z, b_y) > 0
			|| determine_edge(a_x, b_y, a_height, a_depth, b_width, b_depth, a_z, a_y, b_z, b_x) > 0
			|| determine_edge(a_x, b_z, a_height, a_depth, b_width, b_height, a_z, a_y, b_y, b_x) > 0

			|| determine_edge(a_y, b_x, a_width, a_depth, b_height, b_depth, a_z, a_x, b_z, b_y) > 0
			|| determine_edge(a_y, b_y, a_width, a_depth, b_width, b_depth, a_z, a_x, b_z, b_x) > 0
			|| determine_edge(a_y, b_z, a_width, a_depth, b_width, b_height, a_z, a_x, b_y, b_x) > 0

			|| determine_edge(a_z, b_x, a_width, a_height, b_height, b_depth, a_y, a_x, b_z, b_y) > 0
			|| determine_edge(a_z, b_y, a_width, a_height, b_width, b_depth, a_y, a_x, b_z, b_x) > 0
			|| determine_edge(a_z, b_z, a_width, a_height, b_width, b_height, a_y, a_x, b_y, b_x) > 0
			) {
			return true;
		}

		return false;
	}

	Vector3d CollisionsDetector::MinimumDistance(
		const std::shared_ptr<Collider>& first, const Vector3d& first_position,
		const std::shared_ptr<Collider>& second, const Vector3d& second_position,
		const Vector3d& translation_vector)
	{
		const auto first_box = std::dynamic_pointer_cast<BoxCollider>(first);
		const auto second_box = std::dynamic_pointer_cast<BoxCollider>(second);
		if (first_box != nullptr && second_box != nullptr) {
			return MinimumDistance(first_box, first_position, second_box, second_position, translation_vector);
		}

		return Vector3d(0.0f, 0.0f, 0.0f);
	}

	Vector3d CollisionsDetector::MinimumDistance(
		const std::shared_ptr<BoxCollider>& first, const Vector3d& first_position,
		const std::shared_ptr<BoxCollider>& second, const Vector3d& second_position,
		const Vector3d& translation_vector)
	{
		Vector3d minimum_distance_vector(FLOAT_MAX, FLOAT_MAX, FLOAT_MAX);
		float minimum_distance = FLOAT_MAX;
		Vector3d result = translation_vector;

		Vector3d a_proximate, b_proximate;

		const auto a_x = glm::rotate(first->rotation_, Vector3d(1.0f, 0.0f, 0.0f));
		const auto a_y = glm::rotate(first->rotation_, Vector3d(0.0f, 1.0f, 0.0f));
		const auto a_z = glm::rotate(first->rotation_, Vector3d(0.0f, 0.0f, 1.0f));

		const auto b_x = glm::rotate(second->rotation_, Vector3d(1.0f, 0.0f, 0.0f));
		const auto b_y = glm::rotate(second->rotation_, Vector3d(0.0f, 1.0f, 0.0f));
		const auto b_z = glm::rotate(second->rotation_, Vector3d(0.0f, 0.0f, 1.0f));

		auto generate_vertices = [](const Vector3d& center, const Quaternion& rotation, const float& width, const float& height, const float& depth) -> std::vector<Vector3d> {
			std::vector<Vector3d> result;

			result.push_back(center + glm::rotate(rotation, Vector3d(-width, -height, -depth)));
			result.push_back(center + glm::rotate(rotation, Vector3d(-width, height, -depth)));
			result.push_back(center + glm::rotate(rotation, Vector3d(width, height, -depth)));
			result.push_back(center + glm::rotate(rotation, Vector3d(width, -height, -depth)));
			result.push_back(center + glm::rotate(rotation, Vector3d(-width, -height, depth)));
			result.push_back(center + glm::rotate(rotation, Vector3d(-width, height, depth)));
			result.push_back(center + glm::rotate(rotation, Vector3d(width, height, depth)));
			result.push_back(center + glm::rotate(rotation, Vector3d(width, -height, depth)));

			return result;
		};

		const auto a_vertices = generate_vertices(first_position, first->rotation_, first->width_, first->height_, first->depth_);
		const auto b_vertices = generate_vertices(second_position, second->rotation_, second->width_, second->height_, second->depth_);

		auto find_nearest_distance_to_center = [&minimum_distance_vector, &minimum_distance](const std::vector<Vector3d>& vertices_to_check, const Vector3d& other_center) -> Vector3d {
			Vector3d result = minimum_distance_vector;
			for (auto vertex : vertices_to_check) {
				auto vector = vertex - other_center;
				auto distance = glm::length2(vector);
				if (distance < minimum_distance) {
					result = vertex;
					minimum_distance = distance;
				}
			}
			minimum_distance = FLOAT_MAX;

			return result;
		};

		a_proximate = find_nearest_distance_to_center(a_vertices, second_position);
		b_proximate = find_nearest_distance_to_center(b_vertices, first_position);

		minimum_distance_vector = a_proximate - b_proximate;
		minimum_distance = glm::distance2(a_proximate, b_proximate);
		result = translation_vector;

		auto vector_direction = 1.0f;

		auto calculate_determinant = [](const Vector3d& a, const Vector3d& b, const Vector3d& c) -> float {
			return a.x * b.y * c.z + b.x * c.y * a.z + c.x * a.y * b.z - (c.x * b.y * a.z + a.x * c.y * b.z + b.x * a.y * c.z);
		};

		auto try_find_distance_to_plane = [&minimum_distance_vector, &minimum_distance, &vector_direction, &calculate_determinant, &translation_vector, &result](const Vector3d& point_to_check, const Vector3d& plane_point, const Vector3d& plane_vector_a, const Vector3d& plane_vector_b, const std::vector<Vector3d>& other_points) -> void
		{
			const auto plane_normal = glm::normalize(glm::cross(plane_vector_a, plane_vector_b));
			const auto result_vector = point_to_check - plane_point;
			const auto distance = glm::dot(result_vector, plane_normal);
			const auto point_projection = point_to_check - distance * plane_normal;

			Vector3d plane_point_a, plane_point_b;

			for (auto other_point : other_points) {
				if (glm::distance2(other_point, plane_point) <= 1e-4) {
					continue;
				}

				auto vector = plane_point - other_point;
				if (glm::length2(glm::cross(plane_vector_a, vector)) <= 1e-4) {
					plane_point_b = other_point;
				}

				if (glm::length2(glm::cross(plane_vector_b, vector)) <= 1e-4) {
					plane_point_a = other_point;
				}
			}

			const auto plane_point_projection_to_plane_point = plane_point - point_projection;
			const auto plane_point_projection_to_point_a = plane_point_a - point_projection;
			const auto plane_point_projection_to_point_b = plane_point_b - point_projection;

			auto dot1 = glm::dot(plane_vector_a, plane_point_projection_to_plane_point);
			auto dot2 = glm::dot(plane_vector_a, plane_point_projection_to_point_b);
			auto dot3 = glm::dot(plane_vector_b, plane_point_projection_to_plane_point);
			auto dot4 = glm::dot(plane_vector_b, plane_point_projection_to_point_a);
			

			if (glm::dot(plane_vector_a, plane_point_projection_to_plane_point) * glm::dot(plane_vector_a, plane_point_projection_to_point_b) < 0
				&& glm::dot(plane_vector_b, plane_point_projection_to_plane_point) * glm::dot(plane_vector_b, plane_point_projection_to_point_a) < 0
				) 
			{
				if (abs(distance) < minimum_distance) {
					minimum_distance_vector = vector_direction * plane_normal;
					minimum_distance = abs(distance);

					auto determinant = calculate_determinant(plane_normal, plane_vector_a, plane_vector_b);

					auto determinant_plane_normal = calculate_determinant(translation_vector, plane_vector_a, plane_vector_b);
					auto determinant_plane_vector_a = calculate_determinant(plane_normal, translation_vector, plane_vector_b);
					auto determinant_plane_vector_b = calculate_determinant(plane_normal, plane_vector_a, translation_vector);

					auto a_coeff = determinant_plane_vector_a / determinant;
					auto b_coeff = determinant_plane_vector_b / determinant;

					result = a_coeff * plane_vector_a + b_coeff * plane_vector_b + distance * plane_normal;
				}
			}
		};

		try_find_distance_to_plane(a_proximate, b_proximate, b_x, b_y, b_vertices);
		try_find_distance_to_plane(a_proximate, b_proximate, b_x, b_z, b_vertices);
		try_find_distance_to_plane(a_proximate, b_proximate, b_y, b_z, b_vertices);

		vector_direction = -1.0f;
		try_find_distance_to_plane(b_proximate, a_proximate, a_x, a_y, a_vertices);
		try_find_distance_to_plane(b_proximate, a_proximate, a_x, a_z, a_vertices);
		try_find_distance_to_plane(b_proximate, a_proximate, a_y, a_z, a_vertices);
		
		auto try_find_distance_between_lines = [&minimum_distance_vector, &minimum_distance, &a_vertices, &b_vertices, &calculate_determinant, &translation_vector, &result](const Vector3d& first_point, const Vector3d& first_direction_vector, const Vector3d& second_point, const Vector3d& second_direction_vector) -> void {
			const auto points_difference = second_point - first_point;
			auto connection_vector = glm::cross(first_direction_vector, second_direction_vector);
			auto distance = glm::dot(connection_vector, (points_difference)) / glm::length(connection_vector);

			Vector3d first_point_projected, second_point_projected;

			auto determinant = calculate_determinant(first_direction_vector, connection_vector, -second_direction_vector);

			if (abs(determinant) <= 1e-4f) {
				first_point_projected = first_point;
				second_point_projected = second_point;
			}
			else {

				const auto determinant_t_first = calculate_determinant(points_difference, connection_vector, -second_direction_vector);
				const auto determinant_t_connection = calculate_determinant(first_direction_vector, points_difference, -second_direction_vector);
				const auto determinant_t_second = calculate_determinant(first_direction_vector, connection_vector, points_difference);

				const auto t_first = determinant_t_first / determinant;
				const auto t_second = determinant_t_second / determinant;

				first_point_projected = first_point + first_direction_vector * t_first;
				second_point_projected = second_point + second_direction_vector * t_second;
			}

			Vector3d first_other_point, second_other_point;

			for (auto other_point : a_vertices) {
				if (glm::distance2(other_point, first_point) <= 1e-4) {
					continue;
				}

				auto vector = first_point - other_point;
				if (glm::length2(glm::cross(first_direction_vector, vector)) <= 1e-4) {
					first_other_point = other_point;
					break;
				}
			}

			for (auto other_point : b_vertices) {
				if (glm::distance2(other_point, second_point) <= 1e-4) {
					continue;
				}

				auto vector = second_point - other_point;
				if (glm::length2(glm::cross(second_direction_vector, vector)) <= 1e-4) {
					second_other_point = other_point;
					break;
				}
			}

			if (glm::dot(first_point - first_point_projected, first_other_point - first_point_projected) <= 0
				&& glm::dot(second_point - second_point_projected, second_other_point - second_point_projected) <= 0) {

				if (abs(distance) < minimum_distance) {
					minimum_distance_vector = distance * connection_vector;
					minimum_distance = abs(distance);

					const auto other_vector = glm::cross(connection_vector, second_direction_vector);

					determinant = calculate_determinant(connection_vector, second_direction_vector, other_vector);

					const auto second_direction_determinant = calculate_determinant(connection_vector, translation_vector, other_vector);
					const auto other_vector_determinant = calculate_determinant(connection_vector, second_direction_vector, translation_vector);

					const auto a_coeff = second_direction_determinant / determinant;
					const auto b_coeff = other_vector_determinant / determinant;

					result = a_coeff * second_direction_vector + b_coeff * other_vector + distance * connection_vector;
				}
			}
		};

		try_find_distance_between_lines(a_proximate, a_x, b_proximate, b_x);
		try_find_distance_between_lines(a_proximate, a_x, b_proximate, b_y);
		try_find_distance_between_lines(a_proximate, a_x, b_proximate, b_z);

		try_find_distance_between_lines(a_proximate, a_y, b_proximate, b_x);
		try_find_distance_between_lines(a_proximate, a_y, b_proximate, b_y);
		try_find_distance_between_lines(a_proximate, a_y, b_proximate, b_z);

		try_find_distance_between_lines(a_proximate, a_z, b_proximate, b_x);
		try_find_distance_between_lines(a_proximate, a_z, b_proximate, b_y);
		try_find_distance_between_lines(a_proximate, a_z, b_proximate, b_z);

		return result;
	}
}