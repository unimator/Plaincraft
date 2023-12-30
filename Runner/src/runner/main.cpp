#include "input/key_mapping_controller.hpp"
#include <plaincraft_core.hpp>
#include <plaincraft_render_engine_vulkan.hpp>
#include <memory>
#include <iostream>
#include <stdexcept>

using namespace plaincraft_render_engine_vulkan;
using namespace plaincraft_core;
using namespace plaincraft_runner;

int main()
{
	try
	{
		auto scale = 1.6f;
		auto window = std::make_shared<VulkanWindow>("Plaincraft", static_cast<uint32_t>(800 * scale), static_cast<uint32_t>(600 * scale));
		auto render_engine = std::make_unique<VulkanRenderEngine>(window);

		auto game = Game(std::move(render_engine));
		auto key_mapping_controller = KeyMappingController::CreateInstance(game);
		key_mapping_controller->Setup();

		game.Run();
	}
	catch (const std::runtime_error &ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}