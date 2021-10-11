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
#include <plaincraft_common.hpp>
#include <plaincraft_render_engine.hpp>
#include <plaincraft_render_engine_opengl.hpp>
#include <cmath>

namespace plaincraft_core
{
	void WorldGenerator::GenerateWorld(Scene &scene, std::unique_ptr<RenderEngine> &render_engine)
	{
		auto cube_model = read_file_raw("F:/Projekty/Plaincraft/Models/cube_half.obj");
		auto mesh = std::shared_ptr<Mesh>(std::move(Mesh::LoadWavefront(cube_model.data())));

		for (int i = -20; i < 20; ++i)
		{
			for (int j = -20; j < 20; ++j)
			{
				auto entity = std::shared_ptr<Entity>(new Entity());
				const auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\text.png");
				std::shared_ptr<Texture> texture = render_engine->GetTexturesFactory()->LoadFromImage(image);
				auto drawable = std::shared_ptr<Drawable>(new Drawable());
				drawable->SetModel(std::shared_ptr<Model>(new Model(mesh, texture)));
				entity->SetDrawable(drawable);
				auto body = std::shared_ptr<Body>(new Body());

				body->SetCollider(std::shared_ptr<Collider>(new BoxCollider(Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 0.5f, 0.5f, 0.5f)));
				//body->SetPosition(Vector3d(i * 1.0f, sin(i * 0.25f) * cos(j * 0.25f), j * 1.0f));
				entity->SetBody(body);
				entity->SetPosition(Vector3d(i * 1.0f, sin(i * 0.5f) * cos(j * 0.25f), j * 1.0f));
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