#pragma once

class Buffer;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Memory.hpp"
#include "Utils.hpp"

class Buffer {
   public:
    Buffer(VkPhysicalDevice* physicalDevice, VkDevice* device,
           VkDeviceSize size, VkBufferUsageFlags usage,
           VkMemoryPropertyFlags properties);
    VkBuffer* getBuffer();
    void copyTo(Buffer* other, VkQueue graphicsQueue, CommandPool* commandPool);
    Memory* getMemory();
    VkDeviceSize getSize();
    ~Buffer();

   private:
    VkDevice* device;
    VkBuffer* buffer;
    VkDeviceSize size;
    Memory* memory;
};