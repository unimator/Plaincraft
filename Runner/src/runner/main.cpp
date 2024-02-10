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

#include <plaincraft_core.hpp>
#include <plaincraft_render_engine_vulkan.hpp>
#include <memory>
#include <iostream>
#include <stdexcept>

using namespace plaincraft_render_engine_vulkan;
using namespace plaincraft_core;

int main()
{
	try
	{
		auto scale = 1.6f;
		auto window = std::make_shared<VulkanWindow>("Plaincraft", static_cast<uint32_t>(1024 * scale), static_cast<uint32_t>(768 * scale));
		auto render_engine = std::make_unique<VulkanRenderEngine>(window);

		auto game = Game(std::move(render_engine));

		game.Run();
	}
	catch (const std::runtime_error &ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}