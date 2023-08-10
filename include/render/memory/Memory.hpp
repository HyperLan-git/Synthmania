#pragma once

class Memory;

#include "Utils.hpp"

class Memory {
   public:
    Memory(Device& device, VkMemoryRequirements memRequirements,
           VkMemoryPropertyFlags properties);

    Memory(Memory&& other);
    Memory& operator=(Memory&& other);

    Memory(const Memory& other) = delete;
    Memory& operator=(const Memory& other) = delete;

    VkDeviceMemory getMemory();
    void write(const void* data, VkDeviceSize sz, VkDeviceSize offset);
    void read(void* data, VkDeviceSize sz, VkDeviceSize offset);

    ~Memory();

   private:
    Device& device;
    VkDeviceMemory memory;
};
