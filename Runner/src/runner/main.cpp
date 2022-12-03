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
		auto window = std::make_shared<VulkanWindow>("Plaincraft", 800*1.6, 600*1.6);
		auto render_engine = std::make_unique<VulkanRenderEngine>(window);

		auto game = Game(std::move(render_engine));
		auto key_mapping_controller = KeyMappingController::CreateInstance(game);
		key_mapping_controller->Setup();

		game.Run();
	}
	catch(const std::runtime_error& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}