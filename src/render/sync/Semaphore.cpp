#include "Semaphore.hpp"

Semaphore::Semaphore(Device *device) {
    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.flags = 0;
    semaphoreInfo.pNext = NULL;

    type = VK_SEMAPHORE_TYPE_BINARY;
    semaphore = new VkSemaphore();

    if (vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr,
                          semaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphore!");
    }
    this->device = device;
}

Semaphore::Semaphore(Device *device, uint64_t initialValue) {
    VkSemaphoreTypeCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    type = VK_SEMAPHORE_TYPE_TIMELINE;
    info.initialValue = initialValue;
    info.pNext = NULL;

    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = &info;

    info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    semaphore = new VkSemaphore();

    if (vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr,
                          semaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphore!");
    }
    this->device = device;
}

VkSemaphoreType Semaphore::getType() { return type; }

VkSemaphore *Semaphore::getSemaphore() { return semaphore; }

Semaphore::~Semaphore() {
    vkDestroySemaphore(device->getDevice(), *semaphore, nullptr);
    delete semaphore;
}