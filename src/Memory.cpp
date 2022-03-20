#include "Memory.hpp"

Memory::Memory(VkPhysicalDevice* physicalDevice, Device* device,
               VkMemoryRequirements memRequirements,
               VkMemoryPropertyFlags properties) {
    memory = new VkDeviceMemory();
    this->device = device;
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(*(device->getDevice()), &allocInfo, nullptr, memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }
}

VkDeviceMemory* Memory::getMemory() { return memory; }

Memory::~Memory() {
    vkFreeMemory(*(device->getDevice()), *memory, nullptr);
    delete memory;  // I wish I could do that
}