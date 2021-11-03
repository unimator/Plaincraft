#include "input/key_mapping_controller.hpp"
#include <plaincraft_core.hpp>
#include <plaincraft_render_engine_vulkan.hpp>
#include <plaincraft_render_engine_opengl.hpp>
#include <memory>
#include <iostream>

using namespace plaincraft_render_engine_vulkan;
using namespace plaincraft_render_engine_opengl;
using namespace plaincraft_core;
using namespace plaincraft_runner;

int main()
{
	auto window = std::make_shared<VulkanWindow>("Plaincraft", 1024, 768);
	auto render_engine = std::make_unique<VulkanRenderEngine>(window);

	// auto window = std::make_shared<OpenGLWindow>("Plaincraft", 1024, 768);
	// auto render_engine = std::make_unique<OpenGLRenderEngine>(window);

	auto game = Game(std::move(render_engine));
	auto key_mapping_controller = KeyMappingController::CreateInstance();
	key_mapping_controller->Setup(game);

	game.Run();

	return 0;
}