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

#include "world_generator.hpp"
#include "../utils/conversions.hpp"
#include <plaincraft_common.hpp>
#include <plaincraft_render_engine.hpp>
#include <plaincraft_render_engine_opengl.hpp>
#include <cmath>

namespace plaincraft_core
{
	WorldGenerator::WorldGenerator() 
	{}
 
	void WorldGenerator::GenerateWorld(Scene &scene, std::unique_ptr<RenderEngine>& render_engine, ModelsCache& models_cache)
	{
		const auto model = models_cache.Fetch("cube_half");

		for (int i = -10; i < 10; ++i)
		{
			for (int j = -10; j < 10; ++j)
			{
				auto entity = std::make_shared<Entity>(Entity());
				auto drawable = std::make_shared<Drawable>(Drawable());
				drawable->SetModel(model);
				entity->SetDrawable(drawable);
				
				auto position = Vector3d(i * 1.0f, sin(i * 0.25f) * cos(j * 0.25f), j * 1.0f);
				// auto orientation = rp3d::Quaternion::identity();
				// rp3d::Transform transform(rp3d::Vector3(0.0, 0.0, 0.0), orientation);
				// auto cube_shape = physics_common_.createBoxShape(rp3d::Vector3(0.5, 0.5, 0.5));
				// auto rigid_body = physics_world_->createRigidBody(transform);
				// auto collider = rigid_body->addCollider(cube_shape, transform);
				// collider->getMaterial().setBounciness(0);
				// rigid_body->setType(rp3d::BodyType::KINEMATIC);
				// rigid_body->enableGravity(false);
				// rigid_body->setTransform(rp3d::Transform(FromGlm(position), orientation));
				// entity->SetRigidBody(rigid_body);
				
				//body->SetPosition(Vector3d(i * 1.0f, sin(i * 0.25f) * cos(j * 0.25f), j * 1.0f));
				float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				auto color = glm::vec3(r, g, b);
				entity->SetColor(color);

				scene.AddEntity(entity, render_engine);
			}
		}
	}
}