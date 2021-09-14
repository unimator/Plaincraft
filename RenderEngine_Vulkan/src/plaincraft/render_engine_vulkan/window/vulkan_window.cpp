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

#include "vulkan_window.hpp"

namespace plaincraft_render_engine_vulkan
{
    VulkanWindow::VulkanWindow(std::string title, uint32_t width, uint32_t height) 
        : Window(title, width, height)
    {
        Initialize();
    }

	VulkanWindow::VulkanWindow(VulkanWindow&& other) 
		: Window(std::move(other))
	{ 
	}

	VulkanWindow& VulkanWindow::operator=(VulkanWindow&& other)
	{ 
		Window::operator=(std::move(other));
		return *this;
	}

    void VulkanWindow::Initialize() 
    {
        glewExperimental = true;

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		instance_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(instance_, this);
		//glfwSetInputMode(instance_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glfwSetInputMode(instance_, GLFW_STICKY_KEYS, GL_FALSE);

		glEnable(GL_DEPTH_TEST);
		
		glfwSetFramebufferSizeCallback(instance_, FramebufferResizeCallback);
    }

	VkSurfaceKHR VulkanWindow::CreateSurface(VkInstance instance)
	{
		VkSurfaceKHR surface;
		const auto result = glfwCreateWindowSurface(instance, instance_, nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface");
		}
		return surface;
	}

	
	void VulkanWindow::FramebufferResizeCallback(GLFWwindow *instance, int width, int height)
	{
		auto window = reinterpret_cast<VulkanWindow *>(glfwGetWindowUserPointer(instance));
		window->width_ = width;
		window->height_ = height;
		window->was_resized_ = true;
	}
}