#pragma once


#define TINYOBJLOADER_IMPLEMENTATION
#include "Renderer.h"
#include "Window.h"
#include "VulkanApplication.h"

int main()
{
	Renderer r;
	VulkanDeferredApplication va(&r,1000,800);
	return 0;
}
