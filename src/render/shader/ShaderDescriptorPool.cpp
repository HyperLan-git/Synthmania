#include "ShaderDescriptorPool.hpp"

ShaderDescriptorPool::ShaderDescriptorPool(
    Device &device, std::initializer_list<VkDescriptorPoolSize> types)
    : device(device) {
    uint32_t maxSets = 0;
    for (auto it = types.begin(); it != types.end(); it++)
        maxSets += it->descriptorCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = types.size();
    poolInfo.pPoolSizes = types.begin();
    poolInfo.maxSets = maxSets;
    // TODO remove flag for more efficiency
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, NULL, &pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

ShaderDescriptorPool::ShaderDescriptorPool(
    Device &device, const std::vector<VkDescriptorPoolSize> &types)
    : device(device) {
    uint32_t maxSets = 0;
    for (auto it = types.begin(); it != types.end(); it++) {
        maxSets += it->descriptorCount;
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = types.size();
    poolInfo.pPoolSizes = types.data();
    poolInfo.maxSets = maxSets;
    // TODO remove flag for more efficiency
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, NULL, &pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VkDescriptorPool ShaderDescriptorPool::getPool() { return pool; }

Device &ShaderDescriptorPool::getDevice() { return device; }

ShaderDescriptorPool::~ShaderDescriptorPool() {
    vkDestroyDescriptorPool(device.getDevice(), pool, nullptr);
}