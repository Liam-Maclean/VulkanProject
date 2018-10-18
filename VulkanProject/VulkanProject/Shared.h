#pragma once

#include <iostream>
#include <assert.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>


namespace vk
{
	namespace wrappers
	{
		//Wrapper class for buffer holding
		struct Buffer
		{
			VkBuffer buffer;
			VkDeviceMemory memory;
			VkDescriptorBufferInfo descriptor;
		};

		//FrameBuffer attachment Wrapper
		struct FrameBufferAttachment
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
			VkFormat format;
		};
		 
		//FrameBuffer Wrapper
		struct FrameBuffer
		{
			int32_t width, height;
			VkFramebuffer frameBuffer;
			FrameBufferAttachment position, normal, albedo, depth;
			VkRenderPass renderPass;
		};
	}

	//Tools for vulkan
	namespace tools
	{
		void ErrorCheck(VkResult result);
		std::vector<char> ReadShaderFile(const std::string& filename);
	}
}