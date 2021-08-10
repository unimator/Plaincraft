/* 
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Górka

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
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "vulkan_render_engine.hpp"
#include <stdexcept>
#include <iostream>
#include <chrono>
#include "texture/vulkan_textures_factory.hpp"
#include "shader/vulkan_shader.hpp"
#include "renderer/vertex_utils.hpp"
#include "window/vulkan_window.hpp"
#include "utils/queue_family.hpp"
#include "utils/image_utils.hpp"
#include "swapchain/swapchain.hpp"

const std::vector<plaincraft_render_engine::Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4};

namespace plaincraft_render_engine_vulkan
{
	VulkanRenderEngine::VulkanRenderEngine(std::shared_ptr<VulkanWindow> window) : VulkanRenderEngine::VulkanRenderEngine(std::move(window), false) {}

	VulkanRenderEngine::VulkanRenderEngine(std::shared_ptr<VulkanWindow> window, bool enable_debug)
		: RenderEngine(std::move(window)),
		  enable_debug_(enable_debug),
		  instance_(VulkanInstance(VulkanInstanceConfig())),
		  surface_(GetVulkanWindow()->CreateSurface(instance_.GetInstance())),
		  device_(VulkanDevice(instance_, surface_))
	{
		glfwSetFramebufferSizeCallback(window_->GetInstance(), FramebufferResizeCallback);
		swapchain_ = std::make_unique<Swapchain>(GetVulkanWindow(), device_, surface_);
		CreateQueues();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateCommandPool();
		CreateTextureImage();
		CreateTextureImageView();
		CreateTextureSampler();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		CreateSyncObjects();

		textures_factory_ = std::make_shared<VulkanTexturesFactory>(VulkanTexturesFactory());
	}

	VulkanRenderEngine::~VulkanRenderEngine()
	{
		CleanupSwapChain();

		vkDestroySampler(device_.GetDevice(), texture_sampler_, nullptr);
		vkDestroyImageView(device_.GetDevice(), texture_image_view_, nullptr);
		vkDestroyImage(device_.GetDevice(), texture_image_, nullptr);
		vkFreeMemory(device_.GetDevice(), texture_image_memory_, nullptr);

		vkDestroyDescriptorSetLayout(device_.GetDevice(), descriptor_set_layout_, nullptr);

		vkDestroyBuffer(device_.GetDevice(), vertex_buffer_, nullptr);
		vkFreeMemory(device_.GetDevice(), vertex_buffer_memory_, nullptr);

		vkDestroyBuffer(device_.GetDevice(), index_buffer_, nullptr);
		vkFreeMemory(device_.GetDevice(), index_buffer_memory_, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(device_.GetDevice(), render_finished_semaphores_[i], nullptr);
			vkDestroySemaphore(device_.GetDevice(), image_available_semaphores_[i], nullptr);
			vkDestroyFence(device_.GetDevice(), in_flight_fences_[i], nullptr);
		}
		vkDestroyCommandPool(device_.GetDevice(), command_pool_, nullptr);

		vkDestroySurfaceKHR(instance_.GetInstance(), surface_, nullptr);
	}

	void VulkanRenderEngine::CleanupSwapChain()
	{
		for (auto framebuffer : swapchain_->GetSwapchainFrameBuffers())
		{
			vkDestroyFramebuffer(device_.GetDevice(), framebuffer, nullptr);
		}

		vkFreeCommandBuffers(device_.GetDevice(), command_pool_, static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());

		vkDestroyPipeline(device_.GetDevice(), graphics_pipeline_, nullptr);
		vkDestroyPipelineLayout(device_.GetDevice(), pipeline_layout_, nullptr);
		vkDestroyRenderPass(device_.GetDevice(), render_pass_, nullptr);

		for (auto image_view : swapchain_->GetSwapchainImagesViews())
		{
			vkDestroyImageView(device_.GetDevice(), image_view, nullptr);
		}

		vkDestroySwapchainKHR(device_.GetDevice(), swapchain_->GetSwapchain(), nullptr);

		for (size_t i = 0; i < swapchain_->GetSwapchainImages().size(); ++i)
		{
			vkDestroyBuffer(device_.GetDevice(), uniform_buffers_[i], nullptr);
			vkFreeMemory(device_.GetDevice(), uniform_buffers_memory_[i], nullptr);
		}

		vkDestroyDescriptorPool(device_.GetDevice(), descriptor_pool_, nullptr);
	}

	void VulkanRenderEngine::CreateRenderPass()
	{
		VkAttachmentDescription color_attachment{};
		color_attachment.format = swapchain_->GetSwapchainImageFormat();
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

		if (vkCreateRenderPass(device_.GetDevice(), &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS)
		{
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

		VkDescriptorSetLayoutBinding sampler_layout_binding{};
		sampler_layout_binding.binding = 1;
		sampler_layout_binding.descriptorCount = 1;
		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding.pImmutableSamplers = nullptr;
		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {mvp_layout_binding, sampler_layout_binding};
		VkDescriptorSetLayoutCreateInfo layout_info{};
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
		layout_info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device_.GetDevice(), &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout");
		}
	}

	void VulkanRenderEngine::CreateGraphicsPipeline()
	{
		//auto shader = CreateDefaultShader();
		auto vertex_shader_code = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\vert.spv");
		auto fragment_shader_code = read_file_raw("F:\\Projekty\\Plaincraft\\Shaders\\Vulkan\\frag.spv");

		VkShaderModule vertex_shader_module = CreateShaderModule(device_.GetDevice(), vertex_shader_code);
		VkShaderModule fragment_shader_module = CreateShaderModule(device_.GetDevice(), fragment_shader_code);

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

		VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_info, fragment_shader_stage_info};

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

		auto swapchain_extent = swapchain_->GetSwapchainExtent();
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain_extent.width);
		viewport.height = static_cast<float>(swapchain_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = swapchain_extent;

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
		rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

		if (vkCreatePipelineLayout(device_.GetDevice(), &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS)
		{
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

		if (vkCreateGraphicsPipelines(device_.GetDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}

		vkDestroyShaderModule(device_.GetDevice(), fragment_shader_module, nullptr);
		vkDestroyShaderModule(device_.GetDevice(), vertex_shader_module, nullptr);
	}

	void VulkanRenderEngine::CreateFrameBuffers()
	{
		swapchain_->GetSwapchainFrameBuffers().resize(swapchain_->GetSwapchainImagesViews().size());
		auto swapchain_extent = swapchain_->GetSwapchainExtent();

		for (size_t i = 0; i < swapchain_->GetSwapchainImagesViews().size(); ++i)
		{
			VkImageView attachments[] = {
				swapchain_->GetSwapchainImagesViews()[i]};

			VkFramebufferCreateInfo framebuffer_info{};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.renderPass = render_pass_;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = attachments;
			framebuffer_info.width = swapchain_extent.width;
			framebuffer_info.height = swapchain_extent.height;
			framebuffer_info.layers = 1;

			if (vkCreateFramebuffer(device_.GetDevice(), &framebuffer_info, nullptr, &swapchain_->GetSwapchainFrameBuffers()[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer");
			}
		}
	}

	void VulkanRenderEngine::CreateCommandPool()
	{
		QueueFamilyIndices indices = FindQueueFamilyIndices(device_.GetPhysicalDevice(), surface_);

		VkCommandPoolCreateInfo command_pool_info{};
		command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_info.queueFamilyIndex = indices.graphics_family.value();
		command_pool_info.flags = 0;

		if (vkCreateCommandPool(device_.GetDevice(), &command_pool_info, nullptr, &command_pool_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool");
		}
	}

	void VulkanRenderEngine::CreateCommandBuffers()
	{
		command_buffers_.resize(swapchain_->GetSwapchainFrameBuffers().size());

		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.commandPool = command_pool_;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

		if (vkAllocateCommandBuffers(device_.GetDevice(), &allocate_info, command_buffers_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (size_t i = 0; i < command_buffers_.size(); ++i)
		{
			VkCommandBufferBeginInfo buffer_begin_info{};
			buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			buffer_begin_info.flags = 0;
			buffer_begin_info.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(command_buffers_[i], &buffer_begin_info) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = render_pass_;
			render_pass_info.framebuffer = swapchain_->GetSwapchainFrameBuffers()[i];
			render_pass_info.renderArea.offset = {0, 0};
			render_pass_info.renderArea.extent = swapchain_->GetSwapchainExtent();

			VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
			render_pass_info.clearValueCount = 1;
			render_pass_info.pClearValues = &clear_color;

			vkCmdBeginRenderPass(command_buffers_[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

			VkBuffer vertex_buffers[] = {vertex_buffer_};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(command_buffers_[i], 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(command_buffers_[i], index_buffer_, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1, &descriptor_sets_[i], 0, nullptr);
			//vkCmdDraw(command_buffers_[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
			vkCmdDrawIndexed(command_buffers_[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
			vkCmdEndRenderPass(command_buffers_[i]);

			if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer");
			}
		}
	}

	void VulkanRenderEngine::CreateSyncObjects()
	{
		image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
		images_in_flight_.resize(swapchain_->GetSwapchainImages().size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphore_info{};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info{};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS || vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS || vkCreateFence(device_.GetDevice(), &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame");
			}
		}
	}

	void VulkanRenderEngine::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window_->GetInstance(), &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window_->GetInstance(), &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device_.GetDevice());

		CleanupSwapChain();

		swapchain_->RecreateSwapchain();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
	}

	void VulkanRenderEngine::CreateQueues()
	{
		auto indices = FindQueueFamilyIndices(device_.GetPhysicalDevice(), surface_);
		vkGetDeviceQueue(device_.GetDevice(), indices.graphics_family.value(), 0, &graphics_queue_);
		vkGetDeviceQueue(device_.GetDevice(), indices.present_family.value(), 0, &presentation_queue_);
	}

	void VulkanRenderEngine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info)
	{
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
		device_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void *data;
		vkMapMemory(device_.GetDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, vertices.data(), buffer_size);
		vkUnmapMemory(device_.GetDevice(), staging_buffer_memory);

		device_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_, vertex_buffer_memory_);

		CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);

		vkDestroyBuffer(device_.GetDevice(), staging_buffer, nullptr);
		vkFreeMemory(device_.GetDevice(), staging_buffer_memory, nullptr);
	}

	void VulkanRenderEngine::CreateIndexBuffer()
	{
		VkDeviceSize buffer_size = sizeof(uint16_t) * indices.size();

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		device_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void *data;
		vkMapMemory(device_.GetDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, indices.data(), buffer_size);
		vkUnmapMemory(device_.GetDevice(), staging_buffer_memory);

		device_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_, index_buffer_memory_);

		CopyBuffer(staging_buffer, index_buffer_, buffer_size);

		vkDestroyBuffer(device_.GetDevice(), staging_buffer, nullptr);
		vkFreeMemory(device_.GetDevice(), staging_buffer_memory, nullptr);
	}

	void VulkanRenderEngine::CreateUniformBuffers()
	{
		VkDeviceSize buffer_size = sizeof(plaincraft_render_engine::ModelViewProjectionMatrix);

		auto swapchain_images = swapchain_->GetSwapchainImages();
		uniform_buffers_.resize(swapchain_images.size());
		uniform_buffers_memory_.resize(swapchain_images.size());

		for (size_t i = 0; i < swapchain_images.size(); ++i)
		{
			device_.CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_[i], uniform_buffers_memory_[i]);
		}
	}

	
	VkCommandBuffer VulkanRenderEngine::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool = command_pool_;
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(device_.GetDevice(), &allocate_info, &command_buffer);

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &begin_info);

		return command_buffer;
	}

	void VulkanRenderEngine::EndSingleTimeCommands(VkCommandBuffer command_buffer)
	{
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queue_);

		vkFreeCommandBuffers(device_.GetDevice(), command_pool_, 1, &command_buffer);
	}

	void VulkanRenderEngine::CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer, VkDeviceSize size)
	{
		auto command_buffer = BeginSingleTimeCommands();

		VkBufferCopy copy_region{};
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;

		vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &copy_region);

		EndSingleTimeCommands(command_buffer);
	}

	void VulkanRenderEngine::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_image_layout, VkImageLayout new_image_layout)
	{
		auto command_buffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier image_memory_barrier{};
		image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_memory_barrier.oldLayout = old_image_layout;
		image_memory_barrier.newLayout = new_image_layout;
		image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.image = image;
		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_memory_barrier.subresourceRange.baseMipLevel = 0;
		image_memory_barrier.subresourceRange.levelCount = 1;
		image_memory_barrier.subresourceRange.baseArrayLayer = 0;
		image_memory_barrier.subresourceRange.layerCount = 1;
		image_memory_barrier.srcAccessMask = 0;
		image_memory_barrier.dstAccessMask = 0;

		VkPipelineStageFlags source_stage;
		VkPipelineStageFlags destination_stage;

		if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

		EndSingleTimeCommands(command_buffer);
	}

	void VulkanRenderEngine::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		auto command_buffer = BeginSingleTimeCommands();

		VkBufferImageCopy image_copy_region{};
		image_copy_region.bufferOffset = 0;
		image_copy_region.bufferRowLength = 0;
		image_copy_region.bufferImageHeight = 0;

		image_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_copy_region.imageSubresource.mipLevel = 0;
		image_copy_region.imageSubresource.baseArrayLayer = 0;
		image_copy_region.imageSubresource.layerCount = 1;

		image_copy_region.imageOffset = {0, 0, 0};
		image_copy_region.imageExtent = {width, height, 1};

		vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_copy_region);

		EndSingleTimeCommands(command_buffer);
	}

	void VulkanRenderEngine::CreateDescriptorPool()
	{
		auto swapchain_images = swapchain_->GetSwapchainImages();
		std::array<VkDescriptorPoolSize, 2> pool_sizes{};
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = static_cast<uint32_t>(swapchain_images.size());
		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = static_cast<uint32_t>(swapchain_images.size());

		VkDescriptorPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		pool_info.pPoolSizes = pool_sizes.data();
		pool_info.maxSets = static_cast<uint32_t>(swapchain_images.size());

		if (vkCreateDescriptorPool(device_.GetDevice(), &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
	}

	void VulkanRenderEngine::CreateDescriptorSets()
	{
		auto swapchain_images = swapchain_->GetSwapchainImages();
		std::vector<VkDescriptorSetLayout> layouts(swapchain_images.size(), descriptor_set_layout_);
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
		descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_allocate_info.descriptorPool = descriptor_pool_;
		descriptor_set_allocate_info.descriptorSetCount = static_cast<uint32_t>(swapchain_images.size());
		descriptor_set_allocate_info.pSetLayouts = layouts.data();

		descriptor_sets_.resize(swapchain_images.size());
		if (vkAllocateDescriptorSets(device_.GetDevice(), &descriptor_set_allocate_info, descriptor_sets_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor sets");
		}

		for (size_t i = 0; i < swapchain_images.size(); ++i)
		{
			VkDescriptorBufferInfo descriptor_buffer_info{};
			descriptor_buffer_info.buffer = uniform_buffers_[i];
			descriptor_buffer_info.offset = 0;
			descriptor_buffer_info.range = sizeof(ModelViewProjectionMatrix);

			VkDescriptorImageInfo descriptor_image_info{};
			descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptor_image_info.imageView = texture_image_view_;
			descriptor_image_info.sampler = texture_sampler_;

			std::array<VkWriteDescriptorSet, 2> write_descriptor_sets{};
			write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_descriptor_sets[0].dstSet = descriptor_sets_[i];
			write_descriptor_sets[0].dstBinding = 0;
			write_descriptor_sets[0].dstArrayElement = 0;
			write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write_descriptor_sets[0].descriptorCount = 1;
			write_descriptor_sets[0].pBufferInfo = &descriptor_buffer_info;
			write_descriptor_sets[0].pImageInfo = nullptr;
			write_descriptor_sets[0].pTexelBufferView = nullptr;

			write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_descriptor_sets[1].dstSet = descriptor_sets_[i];
			write_descriptor_sets[1].dstBinding = 1;
			write_descriptor_sets[1].dstArrayElement = 0;
			write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write_descriptor_sets[1].descriptorCount = 1;
			write_descriptor_sets[1].pBufferInfo = nullptr;
			write_descriptor_sets[1].pImageInfo = &descriptor_image_info;
			write_descriptor_sets[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(device_.GetDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
		}
	}

	void VulkanRenderEngine::UpdateUniformBuffer(uint32_t image_index)
	{
		static auto start_time = std::chrono::high_resolution_clock::now();

		auto current_time = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

		auto swapchain_extent = swapchain_->GetSwapchainExtent();
		ModelViewProjectionMatrix mvp_matrix{};
		mvp_matrix.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp_matrix.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp_matrix.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 10.0f);
		mvp_matrix.projection[1][1] *= -1;

		void *data;
		vkMapMemory(device_.GetDevice(), uniform_buffers_memory_[image_index], 0, sizeof(mvp_matrix), 0, &data);
		memcpy(data, &mvp_matrix, sizeof(mvp_matrix));
		vkUnmapMemory(device_.GetDevice(), uniform_buffers_memory_[image_index]);
	}

	void VulkanRenderEngine::CreateTextureImage()
	{
		auto image = load_bmp_image_from_file("C:\\Users\\unima\\OneDrive\\Pulpit\\text.png");

		if (!image.data)
		{
			throw std::runtime_error("Failed to load image data");
		}

		VkDeviceSize image_size = image.width * image.height * 4;
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;

		device_.CreateBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
		void *data;
		vkMapMemory(device_.GetDevice(), staging_buffer_memory, 0, image_size, 0, &data);
		memcpy(data, image.data.get(), static_cast<size_t>(image_size));
		vkUnmapMemory(device_.GetDevice(), staging_buffer_memory);

		CreateImage(image.width, image.height, VK_FORMAT_R8G8B8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image_, texture_image_memory_);

		TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(staging_buffer, texture_image_, image.width, image.height);
		TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device_.GetDevice(), staging_buffer, nullptr);
		vkFreeMemory(device_.GetDevice(), staging_buffer_memory, nullptr);
	}

	void VulkanRenderEngine::CreateTextureImageView()
	{
		texture_image_view_ = CreateImageView(device_.GetDevice(), texture_image_, VK_FORMAT_R8G8B8A8_SRGB);
	}

	void VulkanRenderEngine::CreateTextureSampler()
	{
		VkPhysicalDeviceProperties physical_device_properties{};
		vkGetPhysicalDeviceProperties(device_.GetPhysicalDevice(), &physical_device_properties);

		VkSamplerCreateInfo sampler_info{};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_LINEAR;
		sampler_info.minFilter = VK_FILTER_LINEAR;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.anisotropyEnable = VK_TRUE;
		sampler_info.maxAnisotropy = physical_device_properties.limits.maxSamplerAnisotropy;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		if (vkCreateSampler(device_.GetDevice(), &sampler_info, nullptr, &texture_sampler_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture sampler");
		}
	}

	void VulkanRenderEngine::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling image_tiling, VkImageUsageFlags image_usage_flags, VkMemoryPropertyFlags memory_property_flags, VkImage &image, VkDeviceMemory &image_memory)
	{
		VkImageCreateInfo image_create_info{};
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_create_info.imageType = VK_IMAGE_TYPE_2D;
		image_create_info.extent.width = width;
		image_create_info.extent.height = height;
		image_create_info.extent.depth = 1;
		image_create_info.mipLevels = 1;
		image_create_info.arrayLayers = 1;
		image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
		image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_create_info.flags = 0;

		if (vkCreateImage(device_.GetDevice(), &image_create_info, nullptr, &texture_image_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		VkMemoryRequirements memory_requirements;
		vkGetImageMemoryRequirements(device_.GetDevice(), texture_image_, &memory_requirements);

		VkMemoryAllocateInfo memory_allocate_info{};
		memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_allocate_info.allocationSize = memory_requirements.size;
		memory_allocate_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device_.GetDevice(), &memory_allocate_info, nullptr, &texture_image_memory_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory");
		}

		vkBindImageMemory(device_.GetDevice(), texture_image_, texture_image_memory_, 0);
	}

	void VulkanRenderEngine::CreateDepthResources()
	{
	}

	VkShaderModule VulkanRenderEngine::CreateShaderModule(VkDevice device, const std::vector<char> &code)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}

		return shader_module;
	}

	uint32_t VulkanRenderEngine::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_properties)
	{
		VkPhysicalDeviceMemoryProperties device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(device_.GetPhysicalDevice(), &device_memory_properties);

		for (uint32_t i = 0; i < device_memory_properties.memoryTypeCount; ++i)
		{
			if ((type_filter & (1 << i)) && (device_memory_properties.memoryTypes[i].propertyFlags & memory_properties) == memory_properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

	void VulkanRenderEngine::RenderFrame()
	{
		vkWaitForFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

		glfwPollEvents();
		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(device_.GetDevice(), swapchain_->GetSwapchain(), UINT64_MAX, image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		if (images_in_flight_[image_index] != VK_NULL_HANDLE)
		{
			vkWaitForFences(device_.GetDevice(), 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
		}

		images_in_flight_[image_index] = in_flight_fences_[current_frame_];

		UpdateUniformBuffer(image_index);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = {image_available_semaphores_[current_frame_]};
		VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffers_[image_index];

		VkSemaphore signal_semaphores[] = {render_finished_semaphores_[current_frame_]};
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		vkResetFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_]);
		if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submitt draw command buffer");
		}

		VkPresentInfoKHR presentation_info{};
		presentation_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentation_info.waitSemaphoreCount = 1;
		presentation_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swap_chains[] = {swapchain_->GetSwapchain()};
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
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		vkQueueWaitIdle(presentation_queue_);

		current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderEngine::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data)
	{
		std::cerr << "Validation layer: " << callback_data->pMessage << std::endl;

		return VK_FALSE;
	}

	void VulkanRenderEngine::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		auto render_engine = reinterpret_cast<VulkanRenderEngine *>(glfwGetWindowUserPointer(window));
		render_engine->frame_buffer_resized_ = true;
	}
}