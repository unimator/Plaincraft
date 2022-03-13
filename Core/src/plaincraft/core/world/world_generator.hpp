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

#ifndef PLAINCRAFT_CORE_WORLD_GENERATOR
#define PLAINCRAFT_CORE_WORLD_GENERATOR

#include "../common.hpp"
#include "../scene/scene.hpp"
#include "../entities/map/chunk.hpp"
#include <stack>

namespace plaincraft_core
{
	class WorldGenerator
	{
	public:
		rp3d::PhysicsCommon &physics_common_;
		std::shared_ptr<rp3d::PhysicsWorld> physics_world_;
		std::shared_ptr<RenderEngine> render_engine_;
		Scene &scene_;
		Cache<Model> &models_cache_;
		Cache<Texture> &textures_cache_;
		std::stack<rp3d::RigidBody*> free_rigid_bodies_;

	public:
		WorldGenerator(rp3d::PhysicsCommon &physics_common,
					   std::shared_ptr<rp3d::PhysicsWorld> physics_world,
					   std::shared_ptr<RenderEngine> render_engine,
					   Scene &scene,
					   Cache<Model> &models_cache,
					   Cache<Texture> &textures_cache);

		Chunk CreateChunk(I32Vector3d offset);
		void DisposeChunk(std::shared_ptr<Chunk> chunk);
	};
}

#endif // PLAINCRAFT_CORE_WORLD_GENERATOR
