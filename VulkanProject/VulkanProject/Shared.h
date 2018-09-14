#pragma once

#include <iostream>
#include <assert.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>

void ErrorCheck(VkResult result);
std::vector<char> ReadShaderFile(const std::string& filename);