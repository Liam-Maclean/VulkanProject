#include "Window.h"
#include "Renderer.h"
#include "Shared.h"
#include "glm/glm/glm.hpp"
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

struct Light {
	glm::vec4 position;
	glm::vec3 color;
	float radius;
};

struct uboFragmentLights {
	Light lights[6];
	glm::vec4 viewPos;
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

	struct uboVS {
		glm::mat4 projection;
		glm::mat4 model;
		glm::mat4 view;
		glm::vec4 instancePos[3];
	};

	void CreateObjectBuffers();
	void InitialiseVulkanApplication();
	void SceneSetup();
	void Update() override;
	void DrawFrame() override;
	void _CreateGraphicsPipeline() override;
	void _CreateRenderPass() override;
	void _CreateCommandBuffers() override;
	void _CreateDescriptorPool() override;
	void _CreateDescriptorSets() override;
	void _CreateDescriptorSetLayout() override;
	void _CreateVertexBuffer(VkDevice device, const std::vector<Vertex> vertices, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory);
	void _CreateUniformBuffer(VkDevice device, uboVS uboVertexData, VkBuffer* lightBuffer, VkDeviceMemory* lightBufferMemory);
	void _CreateUniformBuffer(VkDevice device, uboFragmentLights uboLightData, VkBuffer * vertexBuffer, VkDeviceMemory * vertexBufferMemory);
	void _CreateVertexBuffer(VkDevice device, directionalLight lightData, VkBuffer* lightBuffer, VkDeviceMemory* lightBufferMemory);
	void _CreateIndexBuffer(VkDevice device, const std::vector<uint32_t> indices, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory);
	void SetUpUniformBuffers();
	
	void CreateGBuffer();
	void CreateDeferredCommandBuffers();

	uboVS offScreenUniformVSData;
	uboVS fullScreenUniformVSData;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;

	vk::wrappers::FrameBuffer deferredOffScreenFrameBuffer;
	VkSampler colorSampler;

	Mesh triangleMesh;

	VkSemaphore offScreenSemaphore;
	VkCommandBuffer offScreenCmdBuffer;

	vk::wrappers::Buffer fullScreenVertexUBOBuffer;
	vk::wrappers::Buffer offScreenVertexUBOBuffer;
	vk::wrappers::Buffer lightUBOBuffer;
	uboFragmentLights lights;

	vk::wrappers::Buffer vertexBuffer;
	vk::wrappers::Buffer lightBuffer;
	vk::wrappers::Buffer indicesBuffer;

};

