#pragma once
#include "Renderer.h"
#include "Window.h"
#include "VulkanApplication.h"
int main()
{
	Renderer r;
	VulkanApplication va(&r,1000,800);
	return 0;
}