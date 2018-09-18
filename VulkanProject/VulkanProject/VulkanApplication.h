#include "Window.h"
#include "Renderer.h"
#include "Shared.h"
#pragma once

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

class VulkanApplication : public VulkanWindow
{
public:
	VulkanApplication(Renderer* renderer, int width, int height)
		:VulkanWindow(renderer, width, height)
	{
		InitialiseVulkanApplication();
	}
	~VulkanApplication();

	void CreateObjectBuffers();
	void InitialiseVulkanApplication();
	void Update() override;
	void _CreateCommandBuffers() override;
	
	directionalLight dLight;

	VkBuffer vertexBuffer;
	VkBuffer lightBuffer;
	VkBuffer indicesBuffer;
	VkDeviceMemory vertexMemory;
	VkDeviceMemory indicesMemory;
	VkDeviceMemory lightBufferMemory;



};

