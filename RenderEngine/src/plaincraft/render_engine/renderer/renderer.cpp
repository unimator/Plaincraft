#include "renderer.hpp"

namespace plaincraft_render_engine {
	Renderer::Renderer(std::shared_ptr<Camera> camera) : camera_(camera) {}

	Renderer::~Renderer() {}

	void Renderer::Batch(std::shared_ptr<Drawable> drawable) {
		drawables_list_.push_back(drawable);
	}

	void Renderer::HasRendered() 
	{
		drawables_list_.clear();
	}
}