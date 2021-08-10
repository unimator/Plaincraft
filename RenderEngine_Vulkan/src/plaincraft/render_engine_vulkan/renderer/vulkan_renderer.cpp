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

#include "vulkan_renderer.hpp"

const std::vector<plaincraft_render_engine::Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

namespace plaincraft_render_engine_vulkan {
    
    VulkanRenderer::VulkanRenderer(VkDevice device, std::shared_ptr<Camera> camera) 
        : Renderer(camera), device_(device)
    {

    }

    void VulkanRenderer::Render() 
    {
        
    }

    void VulkanRenderer::CreateUniformBuffer() 
    {
        VkDeviceSize buffer_size = sizeof(plaincraft_render_engine::ModelViewProjectionMatrix);
    }

    void VulkanRenderer::UpdateUniformBuffer() 
    {
        auto projection = glm::perspective(glm::radians(camera_->fov), (float)1024 / (float)768, 0.1f, 100.0f);
		auto view = glm::lookAt(camera_->position, camera_->position + camera_->direction, camera_->up);

        ModelViewProjectionMatrix mvp_matrix{};
        mvp_matrix.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp_matrix.view = view;
        mvp_matrix.projection = projection;
        
        void* data;
		vkMapMemory(device_, uniform_buffer_memory_, 0, sizeof(mvp_matrix), 0, &data);
		    memcpy(data, &mvp_matrix, sizeof(mvp_matrix));
		vkUnmapMemory(device_, uniform_buffer_memory_);
    }
}