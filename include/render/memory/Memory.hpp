#pragma once

class Memory;

#include "Utils.hpp"

class Memory {
   public:
    Memory(VkPhysicalDevice* physicalDevice, Device* device,
           VkMemoryRequirements memRequirements,
           VkMemoryPropertyFlags properties);

    Memory(const Memory& other) = delete;
    Memory& operator=(const Memory& other) = delete;
    Memory(Memory&& other);
    Memory& operator=(Memory&& other);

    VkDeviceMemory* getMemory();
    void write(const void* data, VkDeviceSize sz, VkDeviceSize offset);
    void read(void* data, VkDeviceSize sz, VkDeviceSize offset);

    ~Memory();

   private:
    Device* device;
    VkDeviceMemory* memory;
};