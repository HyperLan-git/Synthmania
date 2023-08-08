#pragma once

class Buffer;

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Memory.hpp"
#include "Queue.hpp"
#include "Utils.hpp"

class Buffer {
   public:
    Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage,
           VkMemoryPropertyFlags properties);

    Buffer(Buffer&& buf);
    Buffer& operator=(Buffer&& buf);

    Buffer(const Buffer& buf) = delete;
    Buffer& operator=(const Buffer& buf) = delete;

    VkDescriptorBufferInfo createBufferInfo();

    void fill(const void* data);
    void empty(void* data);

    VkBuffer getBuffer();
    VkResult copyTo(Buffer& other, Queue& graphicsQueue, CommandPool& commandPool);
    Memory& getMemory();
    Device& getDevice();
    VkDeviceSize getSize();
    ~Buffer();

   private:
    Device& device;
    VkBuffer buffer;
    VkDeviceSize size;
    std::unique_ptr<Memory> memory;
};