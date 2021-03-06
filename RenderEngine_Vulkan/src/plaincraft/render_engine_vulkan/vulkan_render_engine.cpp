/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin G?rka

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

#define GLM_FORCE_RADIANS
#include "vulkan_render_engine.hpp"
#include <stdexcept>
#include <iostream>
#include <chrono>
#include "texture\vulkan_textures_factory.hpp"
#include "shader\vulkan_shader.hpp"
#include "renderer\vertex_utils.hpp"

const std::vector<plaincraft_render_engine::Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

namespace plaincraft_render_engine_vulkan {
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return static_cast<VkResult>(func(instance, pCreateInfo, pAllocator, pDebugMessenger));
		}
		else {
			return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	VulkanRenderEngine::VulkanRenderEngine(uint32_t width, uint32_t height) : VulkanRenderEngine::VulkanRenderEngine(width, height, false) {}

	VulkanRenderEngine::VulkanRenderEngine(uint32_t width, uint32_t height, bool enable_debug) : RenderEngine(width, height), enable_debug_(enable_debug) {
		CreateWindow();
		CreateVulkanInstance();
		SetupDebugMessanger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapchain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateCommandPool();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateCommandBuffers();
		CreateSyncObjects();

		textures_factory_ = std::make_shared<VulkanTexturesFactory>(VulkanTexturesFactory());
	}

	VulkanRenderEngine::~VulkanRenderEngine() {
		CleanupSwapChain();

		vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);

		vkDestroyBuffer(device_, vertex_buffer_, nullptr);
		vkFreeMemory(device_, vertex_buffer_memory_, nullptr);

		vkDestroyBuffer(device_, index_buffer_, nullptr);
		vkFreeMemory(device_, index_buffer_memory_, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
			vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
			vkDestroyFence(device_, in_flight_fences_[i], nullptr);
		}
		vkDestroyCommandPool(device_, command_pool_, nullptr);

		vkDestroyDevice(device_, nullptr);
		if (enable_debug_) {
			DestroyDebugUtilsMessengerEXT(instance_, debug_messanger_, nullptr);
		}
		vkDestroySurfaceKHR(instance_, surface_, nullptr);
		vkDestroyInstance(instance_, nullptr);

		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void VulkanRenderEngine::CleanupSwapChain()
	{
		for (auto framebuffer : swap_chain_frame_buffers_) {
			vkDestroyFramebuffer(device_, framebuffer, nullptr);
		}

		vkFreeCommandBuffers(device_, command_pool_, static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
		
		vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
		vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
		vkDestroyRenderPass(device_, render_pass_, nullptr);

		for (auto image_view : swap_chain_images_views_) {
			vkDestroyImageView(device_, image_view, nullptr);
		}

		vkDestroySwapchainKHR(device_, swap_chain_, nullptr);

		for (size_t i = 0; swap_chain_images_.size(); ++i) {
			vkDestroyBuffer(device_, uniform_buffers_[i], nullptr);
			vkFreeMemory(device_, uniform_buffers_memory_[i], nullptr);
		}
	}

	void VulkanRenderEngine::CreateWindow() {
		glewExperimental = true;

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window_, this);
		glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);

		//glfwMakeContextCurrent(window_);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glfwSwapInterval(0);
		/*auto init_result = glewInit();
		if (init_result != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			throw std::runtime_error("Failed to initialize GLEW");
		}*/

		//glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_FALSE);

		glEnable(GL_DEPTH_TEST);
	}

	void VulkanRenderEngine::CreateVulkanInstance() {
		if (enable_debug_ && !CheckValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested, but not available!");
		}

		VkApplicationInfo application_info{};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pNext = nullptr;
		application_info.pApplicationName = "Plaincraft Vulkan render engine";
		application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.pEngineName = "No engine";
		application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &application_info;

		auto extensions = GetRequiredExtensions();
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debug_create_info;

		if (enable_debug_) {
			create_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
			create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();

			PopulateDebugMessengerCreateInfo(debug_create_info);
			create_info.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_create_info);
		}
		else {
			create_info.enabledLayerCount = 0;
			create_info.pNext = nullptr;
		}

		if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan instance");
		}
	}

	bool VulkanRenderEngine::CheckValidationLayerSupport()
	{
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		for (const char* layer_name : VALIDATION_LAYERS) {
			bool layer_found = false;

			for (const auto& layer_properties : available_layers) {
				if (strcmp(layer_name, layer_properties.layerName) == 0) {
					layer_found = true;
					break;
				}
			}

			if (!layer_found) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> VulkanRenderEngine::GetRequiredExtensions() {
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		if (enable_debug_) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		uint32_t extensions_count;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());
		
		for (const char* extension : extensions) {
			bool extension_found = false;

			for (const auto& extension_property : available_extensions) {
				if (strcmp(extension, extension_property.extensionName) == 0) {
					extension_found = true;
					break;
				}
			}

			if (!extension_found) {
				throw std::runtime_error("Not all extensions are available");
			}
 		}

		return extensions;
	}

	void VulkanRenderEngine::PickPhysicalDevice() {
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

		if (device_count == 0) {
			throw std::runtime_error("Failed to find GPUs with Vulkan support");
		}

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device)) {
				physical_device_ = device;
				break;
			}
		}

		if (physical_device_ == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to find suitable device");
		}
	}

	void VulkanRenderEngine::CreateSwapchain()
	{
		auto swap_chain_support = QuerySwapChainSupport(physical_device_);

		auto surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
		auto present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
		auto extent = ChooseSwapExtent(swap_chain_support.capabilities);

		auto image_count = swap_chain_support.capabilities.minImageCount + 1;
		if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount) {
			image_count = swap_chain_support.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface_;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		auto indices = FindQueueFamilyIndices(physical_device_);
		uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

		if (indices.graphics_family != indices.present_family) {
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		} else {
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}
		create_info.preTransform = swap_chain_support.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device_, &create_info, nullptr, &swap_chain_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create swapchain");
		}

		vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr);
		swap_chain_images_.resize(image_count);
		vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, swap_chain_images_.data());

		swap_chain_image_format_ = surface_format.format;
		swap_chain_extent_ = extent;
	}

	void VulkanRenderEngine::CreateImageViews()
	{
		swap_chain_images_views_.resize(swap_chain_images_.size());

		for (size_t i = 0; i < swap_chain_images_.size(); ++i) {
			VkImageViewCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = swap_chain_images_[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = swap_chain_image_format_;
			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device_, &create_info, nullptr, &swap_chain_images_views_[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create image views");
			}
		}
	}

	void VulkanRenderEngine::CreateRenderPass()
	{
		VkAttachmentDescription color_attachment{};
		color_attachment.format = swap_chain_image_format_;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_reference{};
		color_attachment_reference.attachment = 0;
		color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass_description{};
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &color_attachment_reference;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


		VkRenderPassCreateInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass_description;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create render pass");
		}
	}

	void VulkanRenderEngine::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding mvp_layout_binding{};
		mvp_layout_binding.binding = 0;
		mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		mvp_layout_binding.descriptorCount = 1;
		mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		mvp_layout_binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo mvp_layout_info{};
		mvp_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		mvp_layout_info.bindingCount = 1;
		mvp_layout_info.pBindings = &mvp_layout_binding;

		if (vkCreateDescriptorSetLayout(device_, &mvp_layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create descriptor set layout");
		}
	}

	void VulkanRenderEngine::CreateGraphicsPipeline()
	{
		//auto shader = CreateDefaultShader();
		auto vertex_shader_code = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\vert.spv");
		auto fragment_shader_code = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\frag.spv");

		VkShaderModule vertex_shader_module = CreateShaderModule(device_, vertex_shader_code);
		VkShaderModule fragment_shader_module = CreateShaderModule(device_, fragment_shader_code);

		VkPipelineShaderStageCreateInfo vertex_shader_stage_info{};
		vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertex_shader_stage_info.module = vertex_shader_module;
		vertex_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo fragment_shader_stage_info{};
		fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragment_shader_stage_info.module = fragment_shader_module;
		fragment_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_stage_info, fragment_shader_stage_info };

		auto binding_description = VertexUtils::GetBindingDescription();
		auto attribute_description = VertexUtils::GetAttributeDescription();

		VkPipelineVertexInputStateCreateInfo vertex_input_info{};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.pVertexBindingDescriptions = &binding_description;
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.size());
		vertex_input_info.pVertexAttributeDescriptions = attribute_description.data();

		VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
		input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly_info.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swap_chain_extent_.width);
		viewport.height = static_cast<float>(swap_chain_extent_.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swap_chain_extent_;

		VkPipelineViewportStateCreateInfo viewport_info{};
		viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_info.viewportCount = 1;
		viewport_info.pViewports = &viewport;
		viewport_info.scissorCount = 1;
		viewport_info.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterization_info{};
		rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_info.depthClampEnable = VK_FALSE;
		rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization_info.lineWidth = 1.0f;
		rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterization_info.depthBiasEnable = VK_FALSE;
		rasterization_info.depthBiasConstantFactor = 0.0f;
		rasterization_info.depthBiasClamp = 0.0f;
		rasterization_info.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisample_info{};
		multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_info.sampleShadingEnable = VK_FALSE;
		multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisample_info.minSampleShading = 1.0f;
		multisample_info.pSampleMask = nullptr;
		multisample_info.alphaToCoverageEnable = VK_FALSE;
		multisample_info.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
		color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment_state.blendEnable = VK_FALSE;
		color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo color_blend_attachment_info{};
		color_blend_attachment_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_attachment_info.logicOpEnable = VK_FALSE;
		color_blend_attachment_info.logicOp = VK_LOGIC_OP_COPY;
		color_blend_attachment_info.attachmentCount = 1;
		color_blend_attachment_info.pAttachments = &color_blend_attachment_state;
		color_blend_attachment_info.blendConstants[0] = 0.0f;
		color_blend_attachment_info.blendConstants[1] = 0.0f;
		color_blend_attachment_info.blendConstants[2] = 0.0f;
		color_blend_attachment_info.blendConstants[3] = 0.0f;

		/*VkDynamicState dynamic_states[] = {
			VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamic_state_info{};
		dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state_info.dynamicStateCount = 2;
		dynamic_state_info.pDynamicStates = dynamic_states;*/

		VkPipelineLayoutCreateInfo pipeline_layout_info{};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 1;
		pipeline_layout_info.pSetLayouts = &descriptor_set_layout_;
		pipeline_layout_info.pushConstantRangeCount = 0;
		pipeline_layout_info.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}

		VkGraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly_info;
		pipeline_info.pViewportState = &viewport_info;
		pipeline_info.pRasterizationState = &rasterization_info;
		pipeline_info.pMultisampleState = &multisample_info;
		pipeline_info.pDepthStencilState = nullptr;
		pipeline_info.pColorBlendState = &color_blend_attachment_info;
		pipeline_info.pDynamicState = nullptr;
		pipeline_info.layout = pipeline_layout_;
		pipeline_info.renderPass = render_pass_;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
		pipeline_info.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphics pipeline");
		}

		vkDestroyShaderModule(device_, fragment_shader_module, nullptr);
		vkDestroyShaderModule(device_, vertex_shader_module, nullptr);
	}

	void VulkanRenderEngine::CreateFrameBuffers()
	{
		swap_chain_frame_buffers_.resize(swap_chain_images_views_.size());

		for (size_t i = 0; i < swap_chain_images_.size(); ++i) {
			VkImageView attachments[] = {
				swap_chain_images_views_[i]
			};

			VkFramebufferCreateInfo framebuffer_info{};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.renderPass = render_pass_;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = attachments;
			framebuffer_info.width = swap_chain_extent_.width;
			framebuffer_info.height = swap_chain_extent_.height;
			framebuffer_info.layers = 1;

			if (vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &swap_chain_frame_buffers_[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create framebuffer");
			}
		}
	}

	void VulkanRenderEngine::CreateCommandPool()
	{
		QueueFamilyIndices indices = FindQueueFamilyIndices(physical_device_);

		VkCommandPoolCreateInfo command_pool_info{};
		command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_info.queueFamilyIndex = indices.graphics_family.value();
		command_pool_info.flags = 0;

		if (vkCreateCommandPool(device_, &command_pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create command pool");
		}
	}

	void VulkanRenderEngine::CreateCommandBuffers()
	{
		command_buffers_.resize(swap_chain_frame_buffers_.size());

		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.commandPool = command_pool_;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

		if (vkAllocateCommandBuffers(device_, &allocate_info, command_buffers_.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (size_t i = 0; i < command_buffers_.size(); ++i) {
			VkCommandBufferBeginInfo buffer_begin_info{};
			buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			buffer_begin_info.flags = 0;
			buffer_begin_info.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(command_buffers_[i], &buffer_begin_info) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = render_pass_;
			render_pass_info.framebuffer = swap_chain_frame_buffers_[i];
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = swap_chain_extent_;

			VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
			render_pass_info.clearValueCount = 1;
			render_pass_info.pClearValues = &clear_color;

			vkCmdBeginRenderPass(command_buffers_[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

			VkBuffer vertex_buffers[] = { vertex_buffer_ };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(command_buffers_[i], 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(command_buffers_[i], index_buffer_, 0, VK_INDEX_TYPE_UINT16);

			//vkCmdDraw(command_buffers_[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
			vkCmdDrawIndexed(command_buffers_[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
			vkCmdEndRenderPass(command_buffers_[i]);

			if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to record command buffer");
			}
		}
	}

	void VulkanRenderEngine::CreateSyncObjects()
	{
		image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
		images_in_flight_.resize(swap_chain_images_.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphore_info{};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info{};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS
				|| vkCreateSemaphore(device_, &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS
				|| vkCreateFence(device_, &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame");
			}
		}
	}

	void VulkanRenderEngine::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window_, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window_, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device_);

		CleanupSwapChain();

		CreateSwapchain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateUniformBuffers();
		CreateCommandBuffers();
	}

	bool VulkanRenderEngine::IsDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = FindQueueFamilyIndices(device);

		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(device, &device_properties);

		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceFeatures(device, &device_features);

		const auto extensions_supported = CheckDeviceExtensionSupport(device);
		auto swap_chain_adequate = false;
		if (extensions_supported) {
			auto swap_chain_support_details = QuerySwapChainSupport(device);
			swap_chain_adequate = !swap_chain_support_details.formats.empty() && !swap_chain_support_details.present_modes.empty();
		}

		return indices.IsComplete() 
			&& device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader
			&& extensions_supported
			&& swap_chain_adequate;
	}

	bool VulkanRenderEngine::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensions_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, nullptr);

		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, available_extensions.data());

		std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());

		for (const auto& extension : available_extensions) {
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	QueueFamilyIndices VulkanRenderEngine::FindQueueFamilyIndices(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

		int i = 0;
		for (const auto& queue_family : queue_families) {
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphics_family = i;
			}

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);

			if (present_support) {
				indices.present_family = i;
			}

			if (indices.IsComplete()) {
				break;
			}

			++i;
		}

		return indices;
	}

	SwapChainSupportDetails VulkanRenderEngine::QuerySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);
		if (format_count != 0) {
			details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats.data());
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);
		if (present_mode_count != 0) {
			details.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, details.present_modes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR VulkanRenderEngine::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
	{
		for (const auto& available_format : available_formats) {
			if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return available_format;
			}
		}

		return available_formats[0];
	}

	VkPresentModeKHR VulkanRenderEngine::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
	{
		for (const auto& available_present_mode : available_present_modes) {
			if (available_present_mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR) {
				return available_present_mode;
			}
		}

		return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanRenderEngine::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(window_, &width, &height);

			VkExtent2D actual_extent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
			actual_extent.height= std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

			return actual_extent;
		}
	}

	void VulkanRenderEngine::CreateLogicalDevice() {
		QueueFamilyIndices indices = FindQueueFamilyIndices(physical_device_);

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

		float queue_priority = 1.0f;
		for (auto queue_family : unique_queue_families) {
			VkDeviceQueueCreateInfo device_queue_create_info{};
			device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_info.queueFamilyIndex = queue_family;
			device_queue_create_info.queueCount = 1;
			device_queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(device_queue_create_info);
		}

		VkPhysicalDeviceFeatures device_feauters{};
		
		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(unique_queue_families.size());
		device_create_info.pEnabledFeatures = &device_feauters;
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions_.size());
		device_create_info.ppEnabledExtensionNames = device_extensions_.data();

		if (enable_debug_) {
			device_create_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
			device_create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
		}
		else {
			device_create_info.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physical_device_, &device_create_info, nullptr, &device_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logical device");
		}

		vkGetDeviceQueue(device_, indices.graphics_family.value(), 0, &graphics_queue_);
		vkGetDeviceQueue(device_, indices.present_family.value(), 0, &presentation_queue_);
	}

	void VulkanRenderEngine::SetupDebugMessanger() {
		if (!enable_debug_) {
			return;
		}

		VkDebugUtilsMessengerCreateInfoEXT create_info;
		PopulateDebugMessengerCreateInfo(create_info);

		if (CreateDebugUtilsMessengerEXT(instance_, &create_info, nullptr, &debug_messanger_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}

	void VulkanRenderEngine::CreateSurface() {
		const auto result = glfwCreateWindowSurface(instance_, window_, nullptr, &surface_);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
 	}

	void VulkanRenderEngine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
		create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = DebugCallback;
		create_info.pUserData = nullptr;
	}

	void VulkanRenderEngine::CreateVertexBuffer()
	{
		VkDeviceSize buffer_size = sizeof(Vertex) * vertices.size();

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void* data;
		vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, vertices.data(), buffer_size);
		vkUnmapMemory(device_, staging_buffer_memory);

		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_, vertex_buffer_memory_);

		CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);

		vkDestroyBuffer(device_, staging_buffer, nullptr);
		vkFreeMemory(device_, staging_buffer_memory, nullptr);
	}

	void VulkanRenderEngine::CreateIndexBuffer()
	{
		VkDeviceSize buffer_size = sizeof(uint16_t) * indices.size();

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void* data;
		vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, indices.data(), buffer_size);
		vkUnmapMemory(device_, staging_buffer_memory);

		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_, index_buffer_memory_);

		CopyBuffer(staging_buffer, index_buffer_, buffer_size);

		vkDestroyBuffer(device_, staging_buffer, nullptr);
		vkFreeMemory(device_, staging_buffer_memory, nullptr);
	}

	void VulkanRenderEngine::CreateUniformBuffers()
	{
		VkDeviceSize buffer_size = sizeof(plaincraft_render_engine::ModelViewProjectionMatrix);

		uniform_buffers_.resize(swap_chain_images_.size());
		uniform_buffers_memory_.resize(swap_chain_images_.size());

		for (size_t i = 0; i < swap_chain_images_.size(); ++i) {
			CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_[i], uniform_buffers_memory_[i]);
		}
	}

	void VulkanRenderEngine::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
	{
		VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vertex buffer");
		}

		VkMemoryRequirements memory_requirements;
		vkGetBufferMemoryRequirements(device_, buffer, &memory_requirements);

		VkMemoryAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.allocationSize = memory_requirements.size;
		allocate_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, memory_properties);

		if (vkAllocateMemory(device_, &allocate_info, nullptr, &buffer_memory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate vertex buffer memory");
		}

		vkBindBufferMemory(device_, buffer, buffer_memory, 0);
	}

	void VulkanRenderEngine::CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool = command_pool_;
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(device_, &allocate_info, &command_buffer);

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &begin_info);

		VkBufferCopy copy_region{};
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;

		vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &copy_region);
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queue_);

		vkFreeCommandBuffers(device_, command_pool_, 1, &command_buffer);
	}

	void VulkanRenderEngine::CreateDescriptorPool()
	{
		VkDescriptorPoolSize pool_size{};
		pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_size.descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());
	}

	void VulkanRenderEngine::UpdateUniformBuffer(uint32_t image_index)
	{
		static auto start_time = std::chrono::high_resolution_clock::now();

		auto current_time = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

		ModelViewProjectionMatrix mvp_matrix{};
		mvp_matrix.model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp_matrix.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp_matrix.projection = glm::perspective(glm::radians(45.0f), swap_chain_extent_.width / (float)swap_chain_extent_.height, 0.1f, 10.0f);
		mvp_matrix.projection[1][1] *= -1;
	
		void* data;
		vkMapMemory(device_, uniform_buffers_memory_[image_index], 0, sizeof(mvp_matrix), 0, &data);
		memcpy(data, &mvp_matrix, sizeof(mvp_matrix));
		vkUnmapMemory(device_, uniform_buffers_memory_[image_index]);
	}

	std::unique_ptr<plaincraft_render_engine::Shader> VulkanRenderEngine::CreateDefaultShader() {
		auto vertex_shader_code = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\vert.spv");
		auto fragment_shader_code = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\frag.spv");

		return std::make_unique<VulkanShader>(std::move(VulkanShader(device_, vertex_shader_code, fragment_shader_code)));
	}

	VkShaderModule VulkanRenderEngine::CreateShaderModule(VkDevice& device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module");
		}

		return shader_module;
	}

	uint32_t VulkanRenderEngine::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_properties)
	{
		VkPhysicalDeviceMemoryProperties device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device_, &device_memory_properties);

		for (uint32_t i = 0; i < device_memory_properties.memoryTypeCount; ++i) {
			if ((type_filter & (1 << i)) && (device_memory_properties.memoryTypes[i].propertyFlags & memory_properties) == memory_properties) {
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

	void VulkanRenderEngine::RenderFrame() 
	{
		vkWaitForFences(device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

		glfwPollEvents();
		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(device_, swap_chain_, UINT64_MAX, image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		if (images_in_flight_[image_index] != VK_NULL_HANDLE) {
			vkWaitForFences(device_, 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
		}

		images_in_flight_[image_index] = in_flight_fences_[current_frame_];

		UpdateUniformBuffer(image_index);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = { image_available_semaphores_[current_frame_] };
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffers_[image_index];

		VkSemaphore signal_semaphores[] = { render_finished_semaphores_[current_frame_] };
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		vkResetFences(device_, 1, &in_flight_fences_[current_frame_]);
		if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to submitt draw command buffer");
		}

		VkPresentInfoKHR presentation_info{};
		presentation_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentation_info.waitSemaphoreCount = 1;
		presentation_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swap_chains[] = { swap_chain_ };
		presentation_info.swapchainCount = 1;
		presentation_info.pSwapchains = swap_chains;
		presentation_info.pImageIndices = &image_index;
		presentation_info.pResults = nullptr;

		result = vkQueuePresentKHR(presentation_queue_, &presentation_info);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frame_buffer_resized_)
		{
			frame_buffer_resized_ = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}

		vkQueueWaitIdle(presentation_queue_);

		current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderEngine::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
		std::cerr << "Validation layer: " << callback_data->pMessage << std::endl;

		return VK_FALSE;
	}

	void VulkanRenderEngine::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto render_engine = reinterpret_cast<VulkanRenderEngine*>(glfwGetWindowUserPointer(window));
		render_engine->frame_buffer_resized_ = true;
	}
}