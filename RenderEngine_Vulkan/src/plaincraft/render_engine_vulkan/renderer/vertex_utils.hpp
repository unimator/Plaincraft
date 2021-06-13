#include <vulkan\vulkan.h>
#include <plaincraft_render_engine.hpp>

#ifndef PLAINCRAFT_RENDER_ENGINE_VULKAN_VERTEX_UTILS
#define PLAINCRAFT_RENDER_ENGINE_VULKAN_VERTEX_UTILS

namespace plaincraft_render_engine_vulkan {
	using namespace plaincraft_render_engine;

	class VertexUtils {
	public:
		static VkVertexInputBindingDescription GetBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescription();
	};
}
#endif // PLAINCRAFT_RENDER_ENGINE_VULKAN_VERTEX_UTILS