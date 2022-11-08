#include "ShaderDescriptorPool.hpp"

ShaderDescriptorPool::ShaderDescriptorPool(Device *device,
                                           VkDescriptorType *types,
                                           uint32_t *typeCounts,
                                           uint32_t count) {
    this->device = device;
    VkDescriptorPoolSize poolSizes[count];
    uint32_t maxSets = 0;
    for (uint32_t i = 0; i < count; i++) {
        poolSizes[i].type = types[i];
        poolSizes[i].descriptorCount = typeCounts[i];
        maxSets += typeCounts[i];
    }
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = count;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = maxSets;
    // TODO remove flag for more efficiency
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    pool = new VkDescriptorPool();

    if (vkCreateDescriptorPool(*(device->getDevice()), &poolInfo, NULL, pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

// TODO remove bad constructor
ShaderDescriptorPool::ShaderDescriptorPool(Device *device,
                                           VkDescriptorType *types,
                                           uint32_t count) {
    this->device = device;
    VkDescriptorPoolSize poolSizes[count];
    uint32_t maxSets = 0;
    for (uint32_t i = 0; i < count; i++) {
        poolSizes[i].type = types[i];
        poolSizes[i].descriptorCount = 64;
        maxSets += poolSizes[i].descriptorCount;
    }
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = count;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = maxSets;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    pool = new VkDescriptorPool();

    if (vkCreateDescriptorPool(*(device->getDevice()), &poolInfo, NULL, pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VkDescriptorPool *ShaderDescriptorPool::getPool() { return pool; }

ShaderDescriptorPool::~ShaderDescriptorPool() {
    vkDestroyDescriptorPool(*(device->getDevice()), *pool, nullptr);
    delete pool;
}