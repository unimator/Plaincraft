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


#ifndef PLAINCRAFT_CORE_MODELS_CACHE
#define PLAINCRAFT_CORE_MODELS_CACHE

#include <plaincraft_render_engine.hpp>
#include <map>
#include <memory>

namespace plaincraft_core {
    using namespace plaincraft_render_engine;

    class ModelsCache {
    private:
        std::map<std::string, std::shared_ptr<Model>> models_;

    public:
        ModelsCache() {}

        ModelsCache(const ModelsCache& other) = delete;
        ModelsCache& operator=(const ModelsCache& other) = delete;

        void Store(std::string name, std::shared_ptr<Model> model);
        std::shared_ptr<Model> Fetch(std::string name);
        void Remove(std::string name);
    };
}

#endif // PLAINCRAFT_CORE_MODELS_CACHE