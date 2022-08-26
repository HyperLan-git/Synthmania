#include "CommandPool.hpp"

CommandPool::CommandPool(Device* device) {
    pool = new VkCommandPool();
    this->device = device;

    VkCommandPoolCreateInfo poolInfo;
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = device->getQueue("main")->getFamily();
    poolInfo.pNext = NULL;

    if (vkCreateCommandPool(*(device->getDevice()), &poolInfo, nullptr, pool) !=
        VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

VkCommandPool* CommandPool::getPool() { return pool; }

CommandPool::~CommandPool() {
    vkDestroyCommandPool(*(device->getDevice()), *pool, nullptr);
    delete pool;
}