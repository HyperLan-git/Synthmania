#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Utils.hpp"

class Buffer {
   public:
    Buffer(VkPhysicalDevice* physicalDevice, VkDevice* device,
           VkDeviceSize size, VkBufferUsageFlags usage,
           VkMemoryPropertyFlags properties, VkDeviceMemory* bufferMemory);
    VkBuffer* getBuffer();
    void copyTo(Buffer* other, VkQueue graphicsQueue,
                VkCommandPool commandPool);

    ~Buffer();

   private:
    VkDevice* device;
    VkBuffer* buffer;
    VkDeviceSize size;
};