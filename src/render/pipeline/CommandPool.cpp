#include "CommandPool.hpp"

CommandPool::CommandPool(VkPhysicalDevice physicalDevice, Device* device) {
    pool = new VkCommandPool();
    this->device = device;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = device->getQueue(0)->getFamily();

    if (vkCreateCommandPool(*(device->getDevice()), &poolInfo, nullptr, pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics command pool!");
    }
}

VkCommandPool* CommandPool::getPool() { return pool; }

CommandPool::~CommandPool() {
    vkDestroyCommandPool(*(device->getDevice()), *pool, nullptr);
    delete pool;
}