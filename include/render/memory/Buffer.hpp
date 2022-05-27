#pragma once

class Buffer;

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Memory.hpp"
#include "Queue.hpp"
#include "Utils.hpp"

class Buffer {
   public:
    Buffer(VkPhysicalDevice* physicalDevice, Device* device, VkDeviceSize size,
           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    VkBuffer* getBuffer();
    void copyTo(Buffer* other, Queue* graphicsQueue, CommandPool* commandPool);
    Memory* getMemory();
    VkDeviceSize getSize();
    ~Buffer();

   private:
    Device* device;
    VkBuffer* buffer;
    VkDeviceSize size;
    Memory* memory;
};