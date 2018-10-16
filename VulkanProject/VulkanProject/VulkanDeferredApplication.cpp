#include "VulkanDeferredApplication.h"

void VulkanDeferredApplication::InitialiseVulkanApplication()
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
	//triangleMesh.LoadMeshFromFile("C:/Users/Liam Maclean/Documents/GitHub/VulkanProject/VulkanProject/VulkanProject/Textures/Avent.obj");
	CreateObjectBuffers();
	_CreateUniformBuffer();
	_CreateDescriptorPool();
	_CreateDescriptorSets();
	_CreateCommandBuffers();
	_CreateSemaphores();
	Update();
}

VulkanDeferredApplication::~VulkanDeferredApplication()
{

	vkDestroyBuffer(_renderer->GetVulkanDevice(), triangleMesh.vertexBuffer.buffer, nullptr);
	vkFreeMemory(_renderer->GetVulkanDevice(), triangleMesh.vertexBuffer.memory, nullptr);

	vkDestroyBuffer(_renderer->GetVulkanDevice(), triangleMesh.indicesBuffer.buffer, nullptr);
	vkFreeMemory(_renderer->GetVulkanDevice(), triangleMesh.indicesBuffer.memory, nullptr);
}

void VulkanDeferredApplication::Update()
{
	VulkanWindow::Update();
}

//Creates the graphics pipelines for the deferred renderer (offscreen and fullscreen pipelines)
void VulkanDeferredApplication::_CreateGraphicsPipeline()
{
	//base forward rendering pipeline creation
	VulkanWindow::_CreateGraphicsPipeline();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.flags = 0;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;

	VkPipelineColorBlendAttachmentState colorBlendState = {};
	colorBlendState.colorWriteMask = 0xf;
	colorBlendState.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo blendStateInfo = {};
	blendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendStateInfo.attachmentCount = 1;
	blendStateInfo.pAttachments = &colorBlendState;

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.stencilTestEnable = VK_TRUE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	VkPipelineViewportStateCreateInfo viewportStateInfo = {};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

	VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {};
	multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	std::vector<VkDynamicState> dynamicStateEnables = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamicStateInfo.pDynamicStates = dynamicStateEnables.data();
	dynamicStateInfo.flags = 0;


	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout = _pipelineLayout[PipelineType::deferred];
	pipelineCreateInfo.renderPass = _renderPass;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineCreateInfo.pColorBlendState = &blendStateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleStateInfo;
	pipelineCreateInfo.pViewportState = &viewportStateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;
	pipelineCreateInfo.pDynamicState = &dynamicStateInfo;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineCreateInfo.pStages = shaderStages.data();

	//Final Pipeline (after offscreen pass)

	//Shader loading (Loads shader modules for pipeline)
	auto deferredVert = vk::tools::ReadShaderFile("M:/GitHub/VulkanProject/VulkanProject/Shaders/deferred.vert.spv");
	auto deferredFrag = vk::tools::ReadShaderFile("M:/GitHub/VulkanProject/VulkanProject/Shaders/deferred.frag.spv");
	auto deferredVert = vk::tools::ReadShaderFile("M:/GitHub/VulkanProject/VulkanProject/Shaders/mrt.vert.spv");
	auto deferredFrag = vk::tools::ReadShaderFile("M:/GitHub/VulkanProject/VulkanProject/Shaders/mrt.frag.spv");
	VkShaderModule deferredVertModule = _CreateShaderModule(deferredVert);
	VkShaderModule deferredFragModule = _CreateShaderModule(deferredFrag);
	VkShaderModule offScreenDeferredVertModule = _CreateShaderModule(deferredVert);
	VkShaderModule offScreenDeferredFragModule = _CreateShaderModule(deferredVert);

	//set up create info handle for vert shader
	VkPipelineShaderStageCreateInfo deferredVertStageCreateInfo = {};
	deferredVertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	deferredVertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	deferredVertStageCreateInfo.module = deferredVertModule;
	deferredVertStageCreateInfo.pName = "Deferred Vert Final-Screen";

	//set up create info handle for vert shader
	VkPipelineShaderStageCreateInfo deferredFragStageCreateInfo = {};
	deferredVertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	deferredVertStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	deferredVertStageCreateInfo.module = deferredFragModule;
	deferredVertStageCreateInfo.pName = "Deferred Frag Final-Screen";

	//set up create info handle for vert shader
	VkPipelineShaderStageCreateInfo offScreenDeferredVertStageCreateInfo = {};
	offScreenDeferredVertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	offScreenDeferredVertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	offScreenDeferredVertStageCreateInfo.module = offScreenDeferredVertModule;
	offScreenDeferredVertStageCreateInfo.pName = "Deferred Vert Off-Screen";

	//set up create info handle for vert shader
	VkPipelineShaderStageCreateInfo offScreenDeferredFragStageCreateInfo = {};
	offScreenDeferredFragStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	offScreenDeferredFragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	offScreenDeferredFragStageCreateInfo.module = offScreenDeferredFragModule;
	offScreenDeferredFragStageCreateInfo.pName = "Deferred Frag Off-Screen";

	shaderStages[0] = deferredVertStageCreateInfo;
	shaderStages[1] = deferredFragStageCreateInfo;

	//Do not do any vertex input, quads are generated by the vertex shader
	VkPipelineVertexInputStateCreateInfo emptyVertexInputState = {};
	emptyVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineCreateInfo.pVertexInputState = &emptyVertexInputState;
	pipelineCreateInfo.layout = _pipelineLayout[PipelineType::deferred];
	vk::tools::ErrorCheck(vkCreateGraphicsPipelines(_renderer->GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipelines[PipelineType::deferred]));

	//Offscreen pass pipeline (before drawing)
	shaderStages[0] = offScreenDeferredVertStageCreateInfo;
	shaderStages[1] = offScreenDeferredFragStageCreateInfo;

	//Swap the render pass and layout to the deferred renderpass and layout (offscreen rendering)
	pipelineCreateInfo.renderPass = deferredOffScreenFrameBuffer.renderPass;
	pipelineCreateInfo.layout = _pipelineLayout[PipelineType::offscreen];


	std::array<VkPipelineColorBlendAttachmentState, 3> blendAttachmentStates;
	//change blend attachments to 0xf (not sure why but example uses it)
	for (int i = 0; i < blendAttachmentStates.size(); i++)
	{
		VkPipelineColorBlendAttachmentState attachmentState = {};
		attachmentState.colorWriteMask = 0xf;
		attachmentState.blendEnable = VK_FALSE;

		blendAttachmentStates[i] = attachmentState;
	}

	//Blend attachments for the pipeline
	blendStateInfo.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
	blendStateInfo.pAttachments = blendAttachmentStates.data();

	vk::tools::ErrorCheck(vkCreateGraphicsPipelines(_renderer->GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipelines[PipelineType::offscreen]));


	//vk::tools::ReadShaderFile()

}


void VulkanDeferredApplication::_CreateRenderPass()
{
	//Base forward rendering render pass creation
	VulkanWindow::_CreateRenderPass();
}

//Method for creating vertex buffer
//*Performs the staging buffer process to convert to GPU memory
void VulkanDeferredApplication::_CreateVertexBuffer(VkDevice device, const std::vector<Vertex> vertices, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory)
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
void VulkanDeferredApplication::_CreateVertexBuffer(VkDevice device, directionalLight lightData, VkBuffer* lightBuffer, VkDeviceMemory* lightBufferMemory)
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
void VulkanDeferredApplication::_CreateIndexBuffer(VkDevice device, const std::vector<uint32_t> indices, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory)
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


//Creates the offscreen framebuffer and attachments for the G-Buffer
void VulkanDeferredApplication::CreateGBuffer()
{
	//Off Screen framebuffer for deferred rendering

	deferredOffScreenFrameBuffer.width = TEX_DIMENSIONS;
	deferredOffScreenFrameBuffer.height = TEX_DIMENSIONS;

	//Create the color attachments for each screen render
	_CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &deferredOffScreenFrameBuffer.position, deferredOffScreenFrameBuffer);
	_CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &deferredOffScreenFrameBuffer.normal, deferredOffScreenFrameBuffer);
	_CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &deferredOffScreenFrameBuffer.albedo, deferredOffScreenFrameBuffer);

	//depth attachment

	//Find Depth Format
	VkFormat DepthFormat;
	DepthFormat = _FindDepthFormat();

	_CreateAttachment(DepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, &deferredOffScreenFrameBuffer.depth, deferredOffScreenFrameBuffer);


	//Attachment descriptions for renderpass 
	std::array<VkAttachmentDescription, 4> attachmentDescs = {};

	for (uint32_t i = 0; i < attachmentDescs.size(); i++)
	{
		attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		//if we're on the depth image description
		if (i = 3)
		{
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		else
		{
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}

	//formats
	attachmentDescs[0].format = deferredOffScreenFrameBuffer.position.format;
	attachmentDescs[1].format = deferredOffScreenFrameBuffer.normal.format;
	attachmentDescs[2].format = deferredOffScreenFrameBuffer.albedo.format;
	attachmentDescs[3].format = deferredOffScreenFrameBuffer.depth.format;

	std::vector<VkAttachmentReference> colorReferences;
	colorReferences.push_back({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	colorReferences.push_back({ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	colorReferences.push_back({ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 3;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pColorAttachments = colorReferences.data();
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
	subpass.pDepthStencilAttachment = &depthReference;

	//Subpass dependencies
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//Create the render pass using the attachments and dependencies data
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pAttachments = attachmentDescs.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 2;
	renderPassInfo.pDependencies = dependencies.data();

	//Create the render pass to go into the frameBuffer struct
	vk::tools::ErrorCheck(vkCreateRenderPass(_renderer->GetVulkanDevice(), &renderPassInfo, nullptr, &deferredOffScreenFrameBuffer.renderPass));

	std::array<VkImageView, 4> attachments;
	attachments[0] = deferredOffScreenFrameBuffer.position.view;
	attachments[1] = deferredOffScreenFrameBuffer.normal.view;
	attachments[2] = deferredOffScreenFrameBuffer.albedo.view;
	attachments[3] = deferredOffScreenFrameBuffer.depth.view;

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = NULL;
	frameBufferCreateInfo.renderPass = deferredOffScreenFrameBuffer.renderPass;
	frameBufferCreateInfo.pAttachments = attachments.data();
	frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	frameBufferCreateInfo.width = deferredOffScreenFrameBuffer.width;
	frameBufferCreateInfo.height = deferredOffScreenFrameBuffer.height;
	frameBufferCreateInfo.layers = 1;
	vk::tools::ErrorCheck(vkCreateFramebuffer(_renderer->GetVulkanDevice(), &frameBufferCreateInfo, nullptr, &deferredOffScreenFrameBuffer.frameBuffer));


	//Create color sampler to sample from the color attachments.
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
	samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 1.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	vk::tools::ErrorCheck(vkCreateSampler(_renderer->GetVulkanDevice(), &samplerCreateInfo, nullptr, &colorSampler));


}

//Creates the deferred command buffers and runs an offscreen frame buffer render pass
void VulkanDeferredApplication::CreateDeferredCommandBuffers()
{
	//if the offscreen cmd buffer hasn't been initialised.
	if (offScreenCmdBuffer == VK_NULL_HANDLE)
	{
		//Create a single command buffer for the offscreen rendering
		VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = _commandPool;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = 1;
	}

	//Set up semaphore create info
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Create a signal semaphore for when the off screen rendering is complete (For pipeline ordering)
	vk::tools::ErrorCheck(vkCreateSemaphore(_renderer->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &offScreenSemaphore));

	//set up cmd buffer begin info
	VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	//Clear values for attachments in fragment shader
	std::array<VkClearValue, 4> clearValues;
	clearValues[0].color = { {0.0f,0.0f,0.0f,0.0f} };
	clearValues[1].color = { { 0.0f,0.0f,0.0f,0.0f } };
	clearValues[2].color = { { 0.0f,0.0f,0.0f,0.0f } };
	clearValues[3].depthStencil = { 1.0f, 0};

	//begin to set up the information for the render pass
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.framebuffer = deferredOffScreenFrameBuffer.frameBuffer;
	renderPassBeginInfo.renderPass = deferredOffScreenFrameBuffer.renderPass;
	renderPassBeginInfo.renderArea.extent.width = deferredOffScreenFrameBuffer.width;
	renderPassBeginInfo.renderArea.extent.height = deferredOffScreenFrameBuffer.height;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();


	//begin command buffer and start the render pass
	vk::tools::ErrorCheck(vkBeginCommandBuffer(offScreenCmdBuffer, &cmdBufferBeginInfo));
	vkCmdBeginRenderPass(offScreenCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	//bind the offscreen graphics pipeline for deferred rendering
	vkCmdBindPipeline(offScreenCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[PipelineType::offscreen]);

	//End the render pass
	vkCmdEndRenderPass(offScreenCmdBuffer);

	//End the command buffer drawing
	vk::tools::ErrorCheck(vkEndCommandBuffer(offScreenCmdBuffer));

}

//Creates the descriptor write and read sets for buffers
void VulkanDeferredApplication::_CreateDescriptorSets()
{
	std::vector<VkWriteDescriptorSet> writeDescriptorSets;

	//For rendering the quad
	VkDescriptorSetAllocateInfo descSetAllocInfo = {};
	descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descSetAllocInfo.descriptorPool = _descriptorPool;
	descSetAllocInfo.descriptorSetCount = 1;
	descSetAllocInfo.pSetLayouts = &descriptorSetLayout;

	vk::tools::ErrorCheck(vkAllocateDescriptorSets(_renderer->GetVulkanDevice(), &descSetAllocInfo, &descriptorSet));


	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = _uniformBuffers;
	buffer_info.offset = 0;
	buffer_info.range = sizeof(UniformBufferObject);


	//Image descriptors for offscreen color attachments
	VkDescriptorImageInfo texDescriptorPosition = {};
	texDescriptorPosition.sampler = colorSampler;
	texDescriptorPosition.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	texDescriptorPosition.imageView = deferredOffScreenFrameBuffer.position.view;

	VkDescriptorImageInfo texDescriptorNormal = {};
	texDescriptorNormal.sampler = colorSampler;
	texDescriptorNormal.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	texDescriptorNormal.imageView = deferredOffScreenFrameBuffer.normal.view;

	VkDescriptorImageInfo texDescriptorAlbedo = {};
	texDescriptorAlbedo.sampler = colorSampler;
	texDescriptorAlbedo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	texDescriptorAlbedo.imageView = deferredOffScreenFrameBuffer.albedo.view;

	//Set up the write descriptor sets

	//Binding 0: Vertex Shader UBO
	VkWriteDescriptorSet uboWriteSet = {};
	uboWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uboWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uboWriteSet.dstSet = _descriptorSets[i];
	uboWriteSet.dstBinding = 0;
	uboWriteSet.dstArrayElement = 0;
	uboWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboWriteSet.descriptorCount = 1;
	uboWriteSet.pBufferInfo = &buffer_info;

	//Binding 1: Position texture for offscreen rendering
	VkWriteDescriptorSet positionShaderWriteSet = {};
	positionShaderWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	positionShaderWriteSet.dstSet = descriptorSet;
	positionShaderWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	positionShaderWriteSet.dstBinding = 1;
	positionShaderWriteSet.pImageInfo = &texDescriptorPosition;
	positionShaderWriteSet.descriptorCount = 1;

	//Binding 2: normal texture for offscreen rendering
	VkWriteDescriptorSet normalShaderWriteSet = {};
	normalShaderWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	normalShaderWriteSet.dstSet = descriptorSet;
	normalShaderWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalShaderWriteSet.dstBinding = 2;
	normalShaderWriteSet.pImageInfo = &texDescriptorNormal;
	normalShaderWriteSet.descriptorCount = 1;

	//Binding 3: albedo texture for offscreen rendering
	VkWriteDescriptorSet albedoShaderWriteSet = {};
	albedoShaderWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	albedoShaderWriteSet.dstSet = descriptorSet;
	albedoShaderWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	albedoShaderWriteSet.dstBinding = 3;
	albedoShaderWriteSet.pImageInfo = &texDescriptorAlbedo;
	albedoShaderWriteSet.descriptorCount = 1;
	writeDescriptorSets = { positionShaderWriteSet, normalShaderWriteSet, albedoShaderWriteSet };
	
	//Binding 4: Lighting shader uniform buffer


	//Update the descriptor sets
	vkUpdateDescriptorSets(_renderer->GetVulkanDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);

	//OffScreen (scene)

}

//Creates descriptor set layouts for Deferred rendering
void VulkanDeferredApplication::_CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding ubo_layout_binding = {};
	ubo_layout_binding.binding = 0;
	ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.descriptorCount = 1;
	ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	ubo_layout_binding.pImmutableSamplers = nullptr;

	//Position layout binding (deferred offscreen buffer sampler)
	VkDescriptorSetLayoutBinding position_layout_binding = {};
	position_layout_binding.binding = 1;
	position_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	position_layout_binding.descriptorCount = 1;
	position_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	position_layout_binding.pImmutableSamplers = nullptr;

	//Normal layout binding (Deferred offscreen buffer sampler)
	VkDescriptorSetLayoutBinding normal_layout_binding = {};
	normal_layout_binding.binding = 2;
	normal_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normal_layout_binding.descriptorCount = 1;
	normal_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	normal_layout_binding.pImmutableSamplers = nullptr;

	//albedo layout binding (Deferred offscreen buffer sampler)
	VkDescriptorSetLayoutBinding albedo_layout_binding = {};
	albedo_layout_binding.binding = 3;
	albedo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	albedo_layout_binding.descriptorCount = 1;
	albedo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	albedo_layout_binding.pImmutableSamplers = nullptr;

	//fragment shader uniform buffer layout
	VkDescriptorSetLayoutBinding fragment_layout_binding = {};
	fragment_layout_binding.binding = 4;
	fragment_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	fragment_layout_binding.descriptorCount = 1;
	fragment_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_layout_binding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = { ubo_layout_binding, position_layout_binding, normal_layout_binding, albedo_layout_binding, fragment_layout_binding };

	VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = {};
	descriptorLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();
	descriptorLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());

	vk::tools::ErrorCheck(vkCreateDescriptorSetLayout(_renderer->GetVulkanDevice(), &descriptorLayoutCreateInfo, nullptr, &descriptorSetLayout));

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.setLayoutCount = 1;

	vk::tools::ErrorCheck(vkCreatePipelineLayout(_renderer->GetVulkanDevice(), &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout[PipelineType::deferred]));
	vk::tools::ErrorCheck(vkCreatePipelineLayout(_renderer->GetVulkanDevice(), &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout[PipelineType::offscreen]));
}

//Method for creating a buffers necessary for rendering 
void VulkanDeferredApplication::CreateObjectBuffers()
{
	VulkanDeferredApplication::_CreateVertexBuffer(_renderer->GetVulkanDevice(), triangleMesh.vertices, &triangleMesh.vertexBuffer.buffer, &triangleMesh.vertexBuffer.memory);
	VulkanDeferredApplication::_CreateIndexBuffer(_renderer->GetVulkanDevice(), triangleMesh.indices, &triangleMesh.indicesBuffer.buffer, &triangleMesh.indicesBuffer.memory);
}

void VulkanDeferredApplication::SceneSetup()
{
	
}

//Method to draw with the command buffers (Override)
void VulkanDeferredApplication::_CreateCommandBuffers()
{
	_drawCommandBuffers.resize(_swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = _commandPool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = (uint32_t)_drawCommandBuffers.size();

	vk::tools::ErrorCheck(vkAllocateCommandBuffers(_renderer->GetVulkanDevice(), &command_buffer_allocate_info, _drawCommandBuffers.data()));

	for (size_t i = 0; i < _drawCommandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo command_buffer_begin_info = {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vk::tools::ErrorCheck(vkBeginCommandBuffer(_drawCommandBuffers[i], &command_buffer_begin_info));

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


		vkCmdBeginRenderPass(_drawCommandBuffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[PipelineType::standard]);

			
			VkBuffer vertexBuffers[] = { triangleMesh.vertexBuffer.buffer};
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(_drawCommandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(_drawCommandBuffers[i], triangleMesh.indicesBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout[PipelineType::standard], 0, 1, &_descriptorSets[i], 0, nullptr);

			vkCmdDrawIndexed(_drawCommandBuffers[i], static_cast<uint32_t>(triangleMesh.indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(_drawCommandBuffers[i]);

		vk::tools::ErrorCheck(vkEndCommandBuffer(_drawCommandBuffers[i]));
	}
}

//Creates descriptor pool
void VulkanDeferredApplication::_CreateDescriptorPool()
{
	VulkanWindow::_CreateDescriptorPool();

	//std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
	//pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//pool_sizes[0].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());
	//pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//pool_sizes[1].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());
	//
	//
	//VkDescriptorPoolCreateInfo pool_create_info = {};
	//pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	//pool_create_info.pPoolSizes = pool_sizes.data();
	//pool_create_info.maxSets = static_cast<uint32_t>(_swapChainImages.size());
	//
	//vk::tools::ErrorCheck(vkCreateDescriptorPool(_renderer->GetVulkanDevice(), &pool_create_info, nullptr, &_descriptorPool));


}

