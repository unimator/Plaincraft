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

#include "scene_builder.hpp"

namespace plaincraft_core
{
    SceneBuilder::SceneBuilder(std::shared_ptr<RenderEngine> render_engine, AssetsManager &assets_manager)
        : render_engine_(render_engine), assets_manager_(assets_manager) {}

    std::unique_ptr<Scene> SceneBuilder::CreateScene()
    {
        auto map = std::make_shared<Map>();
        map->SetName("map");

        PhysicsEngine::PhysicsSettings physics_settings{Vector3d(0.0f, -9.81, 0.0f)};
        auto scene = std::make_unique<Scene>(render_engine_, physics_settings, map);

        scene->AddGameObject(map);

        auto player = std::make_shared<GameObject>();
        player->SetName("player");

        auto player_model = assets_manager_.GetModel("player_cuboid");

        auto drawable = std::make_shared<Drawable>();
        drawable->SetModel(player_model);
        drawable->SetScale(1.0f);
        drawable->SetColor(Vector3d(1.0f, 1.0f, 1.0f));
        player->SetDrawable(drawable);

        auto player_physics_object = std::make_shared<PhysicsObject>();
        auto player_position = Vector3d(8.0f, 55.0f, 16.0f);
        auto player_size = Vector3d(0.8f, 1.8f, 0.8f);
        player_physics_object->position = player_position;
        player_physics_object->size = player_size;
        player_physics_object->friction = 10.0f;
        player_physics_object->type = PhysicsObject::ObjectType::Dynamic;
        player->SetPhysicsObject(player_physics_object);

        scene->AddGameObject(player);
        scene->GetPhysicsEngine().AddObject(player_physics_object);

        return scene;
    }
}