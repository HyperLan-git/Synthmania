#include "CommandBuffer.hpp"

CommandBuffer::CommandBuffer(VkDevice *device, VkCommandPool commandPool) {
    this->device = device;
    this->pool = commandPool;
    this->buffer = new VkCommandBuffer();
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(*device, &allocInfo, buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}
VkCommandBuffer *CommandBuffer::getBuffer() { return buffer; }

void CommandBuffer::reset() {
    vkResetCommandBuffer(*buffer,
                         /*VkCommandBufferResetFlagBits*/ 0);
}

CommandBuffer::~CommandBuffer() {
    vkFreeCommandBuffers(*device, pool, 1, buffer);
    delete buffer;
}