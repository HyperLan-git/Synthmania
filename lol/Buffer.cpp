#include "Buffer.hpp"

Buffer::Buffer(VkPhysicalDevice* physicalDevice, VkDevice* device,
               VkDeviceSize size, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties) {
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

    memory = new Memory(physicalDevice, device, memRequirements, properties);

    vkBindBufferMemory(*device, *buffer, *(memory->getMemory()), 0);
}

void Buffer::copyTo(Buffer* other, VkQueue graphicsQueue,
                    CommandPool* commandPool) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    commandBuffer->begin();

    commandBuffer->copyBufferRegion(this, other, size);

    commandBuffer->end();
    commandBuffer->submit(graphicsQueue);

    delete commandBuffer;
}

Memory* Buffer::getMemory() { return memory; }

VkBuffer* Buffer::getBuffer() { return buffer; }

VkDeviceSize Buffer::getSize() { return size; }

Buffer::~Buffer() {
    vkDestroyBuffer(*device, *buffer, nullptr);
    delete buffer;
    delete memory;  // :(
}