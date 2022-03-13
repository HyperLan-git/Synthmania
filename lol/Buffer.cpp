#include "Buffer.hpp"

Buffer::Buffer(VkPhysicalDevice* physicalDevice, VkDevice* device,
               VkDeviceSize size, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties, VkDeviceMemory* bufferMemory) {
    this->device = device;
    this->buffer = new VkBuffer();
    this->size = size;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(*device, &bufferInfo, nullptr, buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(*device, &allocInfo, nullptr, bufferMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(*device, *buffer, *bufferMemory, 0);
}

void Buffer::copyTo(Buffer* other, VkQueue graphicsQueue,
                    VkCommandPool commandPool) {
    VkCommandBuffer commandBuffer =
        beginSingleTimeCommands(*device, commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, *buffer, *(other->buffer), 1, &copyRegion);

    endSingleTimeCommands(graphicsQueue, *device, commandPool, commandBuffer);
}

VkBuffer* Buffer::getBuffer() { return buffer; }

Buffer::~Buffer() {
    vkDestroyBuffer(*device, *buffer, nullptr);
    delete buffer;
}