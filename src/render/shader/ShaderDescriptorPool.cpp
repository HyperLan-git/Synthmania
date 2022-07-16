#include "ShaderDescriptorPool.hpp"

#include <iostream>
ShaderDescriptorPool::ShaderDescriptorPool(Device *device,
                                           VkDescriptorType *types,
                                           uint32_t count) {
    this->device = device;
    VkDescriptorPoolSize poolSizes[count];
    for (uint32_t i = 0; i < count; i++) {
        poolSizes[i].type = types[i];
        poolSizes[i].descriptorCount = count;
    }
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = count;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = count;

    pool = new VkDescriptorPool();

    if (vkCreateDescriptorPool(*(device->getDevice()), &poolInfo, nullptr,
                               pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VkDescriptorPool *ShaderDescriptorPool::getPool() { return pool; }

ShaderDescriptorPool::~ShaderDescriptorPool() {
    vkDestroyDescriptorPool(*(device->getDevice()), *pool, nullptr);
    delete pool;
}