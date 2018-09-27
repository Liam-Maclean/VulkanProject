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
		};
	}


	namespace tools
	{
		void ErrorCheck(VkResult result);
		std::vector<char> ReadShaderFile(const std::string& filename);
	}
}