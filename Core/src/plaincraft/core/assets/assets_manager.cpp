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

#include "assets_manager.hpp"

namespace plaincraft_core
{
    AssetsManager::AssetsManager(std::shared_ptr<RenderEngine> render_engine) : render_engine_(render_engine) {}

    std::shared_ptr<Model> AssetsManager::GetModel(std::string name)
    {
        if (!models_cache_.Contains(name))
        {
            auto &asset = model_assets[name];
            auto obj_file = read_file_raw(asset.path);
            auto mesh = std::move(Mesh::LoadWavefront(obj_file.data()));
            auto model = render_engine_->GetModelsFactory()->CreateModel(std::move(mesh));
            models_cache_.Store(asset.name, std::move(model));
        }

        return models_cache_.Fetch(name);
    }

    std::shared_ptr<Texture> AssetsManager::GetTexture(std::string name)
    {
        if (!textures_cache_.Contains(name))
        {
            auto &asset = texture_assets[name];
            auto image_file = load_bmp_image_from_file(asset.path);
            auto texture = render_engine_->GetTexturesFactory()->LoadFromImage(image_file);
            textures_cache_.Store(asset.name, std::move(texture));
        }

        return textures_cache_.Fetch(name);
    }
}