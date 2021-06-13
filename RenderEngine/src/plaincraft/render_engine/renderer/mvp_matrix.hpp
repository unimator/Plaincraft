#ifndef PLAINCRAFT_RENDER_ENGINE_MVP_MATRIX
#define PLAINCRAFT_RENDER_ENGINE_MVP_MATRIX

#include "../common.hpp"

namespace plaincraft_render_engine {
	struct ModelViewProjectionMatrix {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};
}
#endif // PLAINCRAFT_RENDER_ENGINE_MVP_MATRIX