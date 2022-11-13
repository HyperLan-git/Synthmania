#include "Buffer.hpp"

Buffer::Buffer(VkPhysicalDevice* physicalDevice, Device* device,
               VkDeviceSize size, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties) {
    this->buffer = new VkBuffer();
    this->device = device;
    this->size = size;

    VkBufferCreateInfo bufferInfo;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags = 0;
    bufferInfo.pQueueFamilyIndices = NULL;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pNext = NULL;

    if (vkCreateBuffer(*(device->getDevice()), &bufferInfo, nullptr, buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*(device->getDevice()), *buffer,
                                  &memRequirements);

    memory = new Memory(physicalDevice, device, memRequirements, properties);

    vkBindBufferMemory(*(device->getDevice()), *buffer, *(memory->getMemory()),
                       0);
}

Buffer::Buffer(Buffer&& buf) {
    this->buffer = buf.buffer;
    this->memory = buf.memory;
    this->device = device;
    this->size = size;
    buf.buffer = NULL;
    buf.memory = NULL;
}

Buffer& Buffer::operator=(Buffer&& buf) {
    if (buffer) {
        vkDestroyBuffer(*(device->getDevice()), *buffer, NULL);
        delete buffer;
    }
    if (memory) delete memory;

    this->buffer = buf.buffer;
    this->memory = buf.memory;
    this->device = device;
    this->size = size;
    buf.buffer = NULL;
    buf.memory = NULL;
    return *this;
}

VkDescriptorBufferInfo* Buffer::createBufferInfo() {
    VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();

    bufferInfo->buffer = *buffer;
    bufferInfo->offset = 0;
    bufferInfo->range = size;
    return bufferInfo;
}

void Buffer::copyTo(Buffer* other, Queue* graphicsQueue,
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
    if (buffer) {
        vkDestroyBuffer(*(device->getDevice()), *buffer, NULL);
        delete buffer;
    }
    if (memory) delete memory;  // :(
}