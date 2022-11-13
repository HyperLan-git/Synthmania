#include "ShaderDescriptorSet.hpp"

ShaderDescriptorSet::ShaderDescriptorSet(Device *device,
                                         ShaderDescriptorPool *pool,
                                         ShaderDescriptorSetLayout *layout) {
    this->pool = pool;
    this->device = device;
    this->set = new VkDescriptorSet();
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = *(pool->getPool());
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layout->getLayout();

    if (vkAllocateDescriptorSets(*(device->getDevice()), &allocInfo, set) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void ShaderDescriptorSet::resetAccess() {
    if (writeDescriptor != NULL) {
        if (writeDescriptor->pNext != NULL)
            delete (VkWriteDescriptorSet *)writeDescriptor->pNext;
        delete writeDescriptor;
    }
    writeDescriptor = NULL;
}

void ShaderDescriptorSet::updateAccess(VkStructureType allowed,
                                       uint32_t binding, VkDescriptorType type,
                                       VkDescriptorBufferInfo *bInfo,
                                       VkDescriptorImageInfo *iInfo) {
    VkWriteDescriptorSet *descriptorWrites = new VkWriteDescriptorSet();

    descriptorWrites->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites->dstSet = *set;
    descriptorWrites->dstBinding = binding;
    descriptorWrites->dstArrayElement = 0;
    descriptorWrites->descriptorType = type;
    descriptorWrites->descriptorCount = 1;
    descriptorWrites->pNext = NULL;

    descriptorWrites->pBufferInfo = bInfo;
    descriptorWrites->pImageInfo = iInfo;

    if (writeDescriptor == NULL) {
        this->writeDescriptor = descriptorWrites;
    } else {
        VkWriteDescriptorSet *cur = writeDescriptor;
        while (cur->pNext != NULL) cur = (VkWriteDescriptorSet *)cur->pNext;

        cur->pNext = descriptorWrites;
    }
    vkUpdateDescriptorSets(*(device->getDevice()), 1, descriptorWrites, 0,
                           NULL);
}

void ShaderDescriptorSet::updateAccess(VkStructureType allowed,
                                       uint32_t binding, VkDescriptorType type,
                                       VkDescriptorBufferInfo *bInfo) {
    updateAccess(allowed, binding, type, bInfo, NULL);
}

void ShaderDescriptorSet::updateAccess(VkStructureType allowed,
                                       uint32_t binding, VkDescriptorType type,
                                       VkDescriptorImageInfo *iInfo) {
    updateAccess(allowed, binding, type, NULL, iInfo);
}

VkDescriptorSet *ShaderDescriptorSet::getSet() { return set; }

ShaderDescriptorSet::~ShaderDescriptorSet() {
    if (writeDescriptor != NULL) {
        if (writeDescriptor->pNext != NULL)
            delete (VkWriteDescriptorSet *)writeDescriptor->pNext;
        delete writeDescriptor;
    }
    vkFreeDescriptorSets(*(device->getDevice()), *(pool->getPool()), 1, set);
    delete set;
}