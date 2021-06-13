#include "../common.hpp"

#ifndef PLAINCRAFT_RENDER_ENGINE_VERTEX
#define PLAINCRAFT_RENDER_ENGINE_VERTEX

namespace plaincraft_render_engine {

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 textCoordinates;
	};
}


#endif // PLAINCRAFT_RENDER_ENGINE_VERTEX