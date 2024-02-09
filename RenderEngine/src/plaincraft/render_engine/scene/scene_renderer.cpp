#include "scene_renderer.hpp"

namespace plaincraft_render_engine {
	SceneRenderer::SceneRenderer(std::shared_ptr<Camera> camera) : camera_(camera) {}

	SceneRenderer::~SceneRenderer() {}

	void SceneRenderer::Batch(std::shared_ptr<Drawable> drawable) {
		if(drawable->GetModel() == nullptr || drawable->GetTexture() == nullptr)
		{
			return;
		}
		drawables_list_.push_back(drawable);
	}

	void SceneRenderer::HasRendered() 
	{
		drawables_list_.clear();
	}
}