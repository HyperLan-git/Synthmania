#pragma once

class Memory;

#include <vulkan/vulkan.h>

#include "Utils.hpp"

class Memory {
   public:
    Memory(VkPhysicalDevice* physicalDevice, Device* device,
           VkMemoryRequirements memRequirements,
           VkMemoryPropertyFlags properties);

    VkDeviceMemory* getMemory();

    ~Memory();

   private:
    Device* device;
    VkDeviceMemory* memory;
};