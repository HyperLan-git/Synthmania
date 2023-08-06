#pragma once

class Buffer;

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Memory.hpp"
#include "Queue.hpp"
#include "Utils.hpp"

class Buffer {
   public:
    Buffer(VkPhysicalDevice* physicalDevice, Device* device, VkDeviceSize size,
           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    Buffer(const Buffer& buf) = delete;
    Buffer& operator=(const Buffer& buf) = delete;

    Buffer(Buffer&& buf);
    Buffer& operator=(Buffer&& buf);

    VkDescriptorBufferInfo createBufferInfo();

    void fill(const void* data);
    void empty(void* data);

    VkBuffer getBuffer();
    void copyTo(Buffer* other, Queue* graphicsQueue, CommandPool* commandPool);
    Memory* getMemory();
    VkDeviceSize getSize();
    ~Buffer();

   private:
    Device* device;
    VkBuffer buffer;
    VkDeviceSize size;
    Memory* memory;
};