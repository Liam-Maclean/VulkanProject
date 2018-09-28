#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <array>
#include "Renderer.h"
#include <set>
#include <map>

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
	glm::vec3 normal;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_description;
	}

	static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions()
	{
		std::array <VkVertexInputAttributeDescription, 4> attribute_descriptions = {};

		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(Vertex, pos);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(Vertex, color);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[2].offset = offsetof(Vertex, texCoord);

		attribute_descriptions[3].binding = 0;
		attribute_descriptions[3].location = 3;
		attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[3].offset = offsetof(Vertex, normal);

		return attribute_descriptions;
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

struct directionalLight
{
	glm::vec3 diffuseColor;
	glm::vec3 direction;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(directionalLight);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_description;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
	{
		std::array <VkVertexInputAttributeDescription, 3> attribute_descriptions = {};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 5;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(directionalLight, diffuseColor);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 6;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(directionalLight, direction);

		return attribute_descriptions;
	}
};

enum PipelineType
{
	standard,
	wireframe,
	cellShaded,
	PBR
};

class VulkanWindow
{
public:
	VulkanWindow(Renderer* renderer, int width, int height);
	~VulkanWindow();

	bool _frameBufferResized = false;
	PipelineType pipelineType = PipelineType::standard;
	

protected:
	//==Suface==
	void _InitSurface();
	void _DeInitSurface();

	//==GLFW Window==
	void _InitWindow(int width, int height);
	void _DeInitWindow();

	//==SwapChain + Framebuffers==
	void _CreateSwapChain();
	void _CreateImageViews();
	void _CreateFramebuffers();
	void _DeInitFramebuffers();
	void _DeInitSwapChain();
	void _DeInitImageViews();
	void _CleanUpSwapChain();
	void _RecreateSwapChain();


	//==Graphics Pipeline==
	void _CreateRenderPass();
	void _CreateGraphicsPipeline();
	void _DeInitRenderPass();
	void _DeInitPipelineLayout();
	void _DeInitGraphicsPipeline();


	//Command pools and buffers
	void _CreateCommandPool();
	void _DeInitCommandPool();

	//Shader Buffers
	void _CreateDescriptorPool();
	void _CreateDescriptorSets();
	void _CreateDescriptorSetLayout();


	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void _CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
	void _CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//Semaphores
	void _CreateSemaphores();
	void _DeInitSemaphores();
	void DrawFrame();

	//Textures
	void _CreateTextureImage();
	void _CreateTextureImageView();
	void _CreateTextureSampler();
	void _CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);


	void _TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void _CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	VkImageView _CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	//Depth stuff
	void _CreateDepthResources();
	VkFormat _FindSupportFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat _FindDepthFormat();
	bool _HasStencilComponent(VkFormat format);


	//==Renderer==
	Renderer * _renderer;

	//Methods
	virtual void Update();
	virtual void _CreateCommandBuffers();
	virtual void _UpdateUniformBuffer(uint32_t imageIndex);
	virtual void _CreateUniformBuffer();


	//utility
	SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR _ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device);
	VkShaderModule _CreateShaderModule(const std::vector<char>& code);

	VkCommandBuffer _BeginSingleTimeCommands();
	void _EndSingleTimeCommands(VkCommandBuffer commandBuffer);


	//Variables
	//GLFW
	GLFWwindow* _window;

	//===VK===
	//Surface
	VkSurfaceKHR _surface;

	//==Swapchain==
	VkSwapchainKHR _swapChain;
	std::vector<VkImage> _swapChainImages;
	std::vector<VkImageView> _swapChainImageViews;
	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	std::vector <VkFramebuffer> _swapChainFramebuffers;

	
	//Graphics Pipeline
	std::map<PipelineType, VkPipeline> graphicsPipelines;
	VkPipeline _graphicsPipeline;
	VkRenderPass _renderPass;
	VkDescriptorSetLayout _descriptorSetLayout;
	VkPipelineLayout _pipelineLayout;

	//vertex and index buffers
	VkBuffer _vertexBuffer;
	VkDeviceMemory _vertexBufferMemory;
	VkBuffer _indexBuffer;
	VkDeviceMemory _indexBufferMemory;
	std::vector <VkBuffer> _uniformBuffers;
	std::vector <VkDeviceMemory> _uniformBuffersMemory;
	VkDescriptorPool _descriptorPool;
	std::vector<VkDescriptorSet> _descriptorSets;

	//Command pools and buffers
	VkCommandPool _commandPool;
	std::vector<VkCommandBuffer> _commandBuffers;

	//Semaphores
	size_t currentFrame = 0;
	const int MAX_FRAMES_IN_FLIGHT = 2;
	std::vector<VkSemaphore> _imageAvailableSemaphores;
	std::vector<VkSemaphore> _renderFinishedSemaphores;
	std::vector<VkFence> _inFlightFences;

	//Texture sampling and images
	VkImage _textureImage;
	VkImageView _textureImageView;
	VkSampler _textureSampler;
	VkDeviceMemory _textureImageMemory;


	std::vector<VkImage> _depthImages;
	std::vector<VkDeviceMemory> _depthImagesMemory;
	std::vector<VkImageView> _depthImagesView;


	VkDynamicState _dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};
private:

	



};

