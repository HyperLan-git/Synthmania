#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"

uint32_t findMemoryType(VkPhysicalDevice *physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);
std::vector<char> readFile(const std::string &filename);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);