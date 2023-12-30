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

#include "simple_chunk_builder.hpp"
#include "../../entities/blocks/stone.hpp"

namespace plaincraft_core
{
    SimpleChunkBuilder::SimpleChunkBuilder(Scene &scene) : scene_(scene)
    {
    }

    bool SimpleChunkBuilder::GenerateChunkStep(std::shared_ptr<Chunk> chunk)
    {
        auto chunk_x = chunk->GetPositionX();
        auto chunk_z = chunk->GetPositionZ();

        std::shared_ptr<Block> game_object;
        game_object = std::make_shared<Stone>();
        auto physics_object = std::make_shared<PhysicsObject>();
        physics_object->size = Vector3d(1.0f, 1.0f, 1.0f);
        physics_object->type = PhysicsObject::ObjectType::Static;
        auto offset = Vector3d(8.0f, 15.0f, 8.0f);
        physics_object->position = Vector3d(chunk_x, 0.0f, chunk_z) + offset;
        game_object->SetPhysicsObject(physics_object);
        chunk->blocks_[8][15][8] = game_object;

        chunk->initialized_ = true;
        return true;
    }

    bool SimpleChunkBuilder::DisposeChunkStep(std::shared_ptr<Chunk> chunk)
    {
        scene_.RemoveGameObject(chunk);
        auto &blocks = chunk->GetData();
        auto &block = blocks[8][15][8];
        if (block != nullptr)
        {
            scene_.RemoveGameObject(block);
        }
        return true;
    }
}