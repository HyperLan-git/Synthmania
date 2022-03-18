#pragma once

class Memory;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utils.hpp"

class Memory {
   public:
    Memory(VkPhysicalDevice* physicalDevice, VkDevice* device,
           VkMemoryRequirements memRequirements,
           VkMemoryPropertyFlags properties);

    VkDeviceMemory* getMemory();

    ~Memory();

   private:
    VkDevice* device;
    VkDeviceMemory* memory;
};