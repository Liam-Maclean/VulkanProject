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
	triangleMesh.LoadMeshFromFile("C:/Users/Liam Maclean/Documents/GitHub/VulkanProject/VulkanProject/VulkanProject/Textures/Avent.obj");
	CreateObjectBuffers();
	_CreateUniformBuffer();
	_CreateDescriptorPool();
	_CreateDescriptorSets();
	_CreateCommandBuffers();
	_CreateSemaphores();
	Update();
}

VulkanApplication::~VulkanApplication()
{

	vkDestroyBuffer(_renderer->GetVulkanDevice(), triangleMesh.vertexBuffer.buffer, nullptr);
	vkFreeMemory(_renderer->GetVulkanDevice(), triangleMesh.vertexBuffer.memory, nullptr);

	vkDestroyBuffer(_renderer->GetVulkanDevice(), triangleMesh.indicesBuffer.buffer, nullptr);
	vkFreeMemory(_renderer->GetVulkanDevice(), triangleMesh.indicesBuffer.memory, nullptr);
}

void VulkanApplication::Update()
{
	VulkanWindow::Update();
}

//Method for creating vertex buffer
//*Performs the staging buffer process to convert to GPU memory
void VulkanApplication::_CreateVertexBuffer(VkDevice device, const std::vector<Vertex> vertices, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	_CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(_renderer->GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(_renderer->GetVulkanDevice(), stagingBufferMemory);

	_CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *vertexBuffer, *vertexBufferMemory);
	_CopyBuffer(stagingBuffer, *vertexBuffer, bufferSize);

	vkDestroyBuffer(_renderer->GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(_renderer->GetVulkanDevice(), stagingBufferMemory, nullptr);
}

//Method for creating vertex buffer for lighting
//*Performs the staging buffer process to convert to GPU memory
void VulkanApplication::_CreateVertexBuffer(VkDevice device, directionalLight lightData, VkBuffer* lightBuffer, VkDeviceMemory* lightBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(lightData);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	_CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(_renderer->GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &lightData, (size_t)bufferSize);
	vkUnmapMemory(_renderer->GetVulkanDevice(), stagingBufferMemory);

	_CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *lightBuffer, *lightBufferMemory);
	_CopyBuffer(stagingBuffer, *lightBuffer, bufferSize);

	vkDestroyBuffer(_renderer->GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(_renderer->GetVulkanDevice(), stagingBufferMemory, nullptr);
}

//Method for creating index buffer
//*Performs the staging buffer process to convert to GPU memory
void VulkanApplication::_CreateIndexBuffer(VkDevice device, const std::vector<uint32_t> indices, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	_CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(_renderer->GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(_renderer->GetVulkanDevice(), stagingBufferMemory);

	_CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffer, *indexBufferMemory);

	_CopyBuffer(stagingBuffer, *indexBuffer, bufferSize);

	vkDestroyBuffer(_renderer->GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(_renderer->GetVulkanDevice(), stagingBufferMemory, nullptr);
}

//Method for creating a buffers necessary for rendering 
void VulkanApplication::CreateObjectBuffers()
{
	VulkanApplication::_CreateVertexBuffer(_renderer->GetVulkanDevice(), triangleMesh.vertices, &triangleMesh.vertexBuffer.buffer, &triangleMesh.vertexBuffer.memory);
	VulkanApplication::_CreateIndexBuffer(_renderer->GetVulkanDevice(), triangleMesh.indices, &triangleMesh.indicesBuffer.buffer, &triangleMesh.indicesBuffer.memory);
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

			
			VkBuffer vertexBuffers[] = { triangleMesh.vertexBuffer.buffer};
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(_commandBuffers[i], triangleMesh.indicesBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[i], 0, nullptr);

			vkCmdDrawIndexed(_commandBuffers[i], static_cast<uint32_t>(triangleMesh.indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(_commandBuffers[i]);

		vk::tools::ErrorCheck(vkEndCommandBuffer(_commandBuffers[i]));
	}
}
