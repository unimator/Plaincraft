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

#ifndef PLAINCRAFT_RUNNER_KEYMAPPING_CONTROLLER
#define PLAINCRAFT_RUNNER_KEYMAPPING_CONTROLLER

#include <plaincraft_core.hpp>
#include <memory>

using namespace plaincraft_core;

namespace plaincraft_runner
{
    class KeyMappingController : public EventObserver, public std::enable_shared_from_this<KeyMappingController>
    {
    private:
        std::shared_ptr<Entity> player_;
        std::shared_ptr<Camera> camera_;

        float movement_speed_ = 3.0f;
        float maximum_speed_ = 3.0f;

    public:
        static std::shared_ptr<KeyMappingController> CreateInstance();

        void Setup(Game &game_instance);

        void OnEventTriggered(const Event &event) override;

    private:
        KeyMappingController() = default;
    };
}

#endif // PLAINCRAFT_RUNNER_KEYMAPPING_CONTROLLER