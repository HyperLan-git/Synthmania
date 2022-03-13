#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

uint32_t findMemoryType(VkPhysicalDevice *physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);
std::vector<char> readFile(const std::string &filename);

VkCommandBuffer beginSingleTimeCommands(VkDevice device,
                                        VkCommandPool commandPool);

void endSingleTimeCommands(VkQueue graphicsQueue, VkDevice device,
                           VkCommandPool commandPool,
                           VkCommandBuffer commandBuffer);