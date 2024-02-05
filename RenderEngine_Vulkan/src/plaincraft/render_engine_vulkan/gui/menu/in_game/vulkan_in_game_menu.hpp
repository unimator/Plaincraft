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

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IN_GAME_MENU
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IN_GAME_MENU

#include "../vulkan_menu.hpp"

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <memory>

namespace plaincraft_render_engine_vulkan
{
    class VulkanInGameMenu final : public VulkanMenu
    {
    private:
        std::shared_ptr<ImFont> standard_font_;

    public:
        VulkanInGameMenu(std::shared_ptr<ImFont> standard_font);

        void Draw(const FrameConfig &frame_config) override;
    };
}

#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VULKAN_IN_GAME_MENU