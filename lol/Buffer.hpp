#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

class Buffer {
   public:
    Buffer(VkDevice* device, VkDeviceSize size, VkBufferUsageFlags usage,
           VkMemoryPropertyFlags properties, VkDeviceMemory* bufferMemory);

    VkBuffer* getBuffer();

    ~Buffer();

   private:
    VkDevice* device;
    VkBuffer* buffer;
}