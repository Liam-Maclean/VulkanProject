#include "Window.h"
#include "Renderer.h"
#include "Shared.h"
#include "BaseModel.h"
#pragma once

#define TEX_DIMENSIONS 2048;

const std::vector<Vertex> vertices = {
	{{ -0.5f, -0.5f, 0.0f},{ 1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f}},
	{{  0.5f, -0.5f, 0.0f},{ 0.0f, 1.0f, 0.0f}, { 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f }},
	{{  0.5f, 0.5f, 0.0f},{ 0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f },{ 0.0f, 1.0f, 0.0f }},
	{{ -0.5f, 0.5f, 0.0f},{ 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f }}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
	//4, 5, 6, 6, 7, 4
};


class VulkanDeferredApplication : public VulkanWindow
{
public:
	VulkanDeferredApplication(Renderer* renderer, int width, int height)
		:VulkanWindow(renderer, width, height)
	{
		InitialiseVulkanApplication();
	}
	~VulkanDeferredApplication();

	void CreateObjectBuffers();
	void InitialiseVulkanApplication();
	void SceneSetup();
	void Update() override;
	void _CreateGraphicsPipeline() override;
	void _CreateRenderPass() override;
	void _CreateCommandBuffers() override;
	void _CreateDescriptorSetLayout() override;
	void _CreateVertexBuffer(VkDevice device, const std::vector<Vertex> vertices, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory);
	void _CreateVertexBuffer(VkDevice device, directionalLight lightData, VkBuffer* lightBuffer, VkDeviceMemory* lightBufferMemory);
	void _CreateIndexBuffer(VkDevice device, const std::vector<uint32_t> indices, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory);
	void CreateGBuffer();
	void CreateDeferredCommandBuffers();

	VkDescriptorSetLayout descriptorSetLayout;


	vk::wrappers::FrameBuffer deferredOffScreenFrameBuffer;
	VkSampler colorSampler;

	Mesh triangleMesh;

	VkCommandBuffer offScreenCmdBuffer;

	directionalLight dLight;
	vk::wrappers::Buffer vertexBuffer;
	vk::wrappers::Buffer lightBuffer;
	vk::wrappers::Buffer indicesBuffer;

};

