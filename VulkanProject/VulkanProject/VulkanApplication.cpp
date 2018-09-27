#include "VulkanApplication.h"

void VulkanApplication::InitialiseVulkanApplication()
{
	_InitSurface();
	_CreateSwapChain();
	_CreateImageViews();
	_CreateRenderPass();
	_CreateDescriptorSetLayout();
	_CreateGraphicsPipeline();
	_CreateCommandPool();
	_CreateDepthResources();
	_CreateFramebuffers();
	_CreateTextureImage();
	_CreateTextureImageView();
	_CreateTextureSampler();
	CreateObjectBuffers();
	_CreateUniformBuffer();
	_CreateDescriptorPool();
	_CreateDescriptorSets();
	_CreateCommandBuffers();
	_CreateSemaphores();
	Update();


	dLight.diffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);
	dLight.direction = glm::vec3(-0.5f, -0.5f, 0.0f);

	
}

VulkanApplication::~VulkanApplication()
{

}

void VulkanApplication::Update()
{
	VulkanWindow::Update();
}

//Method for creating a buffers necessary for rendering 
void VulkanApplication::CreateObjectBuffers()
{
	VulkanWindow::_CreateVertexBuffer(vertices, &vertexBuffer.buffer, &vertexBuffer.memory);
	VulkanWindow::_CreateIndexBuffer(indices, &indicesBuffer.buffer, &indicesBuffer.memory);
}

//Method to draw with the command buffers (Override)
void VulkanApplication::_CreateCommandBuffers()
{
	_commandBuffers.resize(_swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = _commandPool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = (uint32_t)_commandBuffers.size();

	vk::tools::ErrorCheck(vkAllocateCommandBuffers(_renderer->GetVulkanDevice(), &command_buffer_allocate_info, _commandBuffers.data()));

	for (size_t i = 0; i < _commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo command_buffer_begin_info = {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vk::tools::ErrorCheck(vkBeginCommandBuffer(_commandBuffers[i], &command_buffer_begin_info));

		VkRenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = _renderPass;
		render_pass_begin_info.framebuffer = _swapChainFramebuffers[i];
		render_pass_begin_info.renderArea.offset = { 0,0 };
		render_pass_begin_info.renderArea.extent = _swapChainExtent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
		render_pass_begin_info.pClearValues = clearValues.data();


		vkCmdBeginRenderPass(_commandBuffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[PipelineType::standard]);

			VkBuffer vertexBuffers[] = { vertexBuffer.buffer};
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, vertexBuffers, offsets);
	
			vkCmdBindIndexBuffer(_commandBuffers[i], indicesBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[i], 0, nullptr);

			vkCmdDrawIndexed(_commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(_commandBuffers[i]);

		vk::tools::ErrorCheck(vkEndCommandBuffer(_commandBuffers[i]));
	}
}
