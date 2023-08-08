#include "Buffer.hpp"

Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties)
    : device(device) {
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

    if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.getDevice(), buffer, &memRequirements);

    memory = std::make_unique<Memory>(device, memRequirements, properties);

    vkBindBufferMemory(device.getDevice(), buffer, memory->getMemory(), 0);
}

Buffer::Buffer(Buffer&& buf)
    : device(buf.device), buffer(NULL), memory(), size(buf.size) {
    *this = std::move(buf);
}

Buffer& Buffer::operator=(Buffer&& buf) {
    assert(buf.device == this->device);
    std::swap(this->buffer, buf.buffer);
    std::swap(this->memory, buf.memory);
    return *this;
}

VkDescriptorBufferInfo Buffer::createBufferInfo() {
    VkDescriptorBufferInfo bufferInfo;

    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = size;
    return bufferInfo;
}

void Buffer::fill(const void* data) {
    this->memory->write(data, this->size, 0);
}

Device& Buffer::getDevice() { return device; }

void Buffer::empty(void* data) { this->memory->read(data, this->size, 0); }

VkResult Buffer::copyTo(Buffer& other, Queue& graphicsQueue,
                        CommandPool& commandPool) {
    CommandBuffer commandBuffer(commandPool, true);
    return_if(beg, commandBuffer.begin());

    commandBuffer.copyBufferRegion(*this, other, size);

    return_if(end, commandBuffer.end());
    return commandBuffer.submit(graphicsQueue);
}

Memory& Buffer::getMemory() { return *memory; }

VkBuffer Buffer::getBuffer() { return buffer; }

VkDeviceSize Buffer::getSize() { return size; }

Buffer::~Buffer() {
    if (buffer) vkDestroyBuffer(device.getDevice(), buffer, NULL);
}