#include "Semaphore.hpp"

Semaphore::Semaphore(Device *device) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    semaphore = new VkSemaphore();

    if (vkCreateSemaphore(*(device->getDevice()), &semaphoreInfo, nullptr,
                          semaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphore!");
    }
    this->device = device;
}

VkSemaphore *Semaphore::getSemaphore() { return semaphore; }

Semaphore::~Semaphore() {
    vkDestroySemaphore(*(device->getDevice()), *semaphore, nullptr);
    delete semaphore;
}