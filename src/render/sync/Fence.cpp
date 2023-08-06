#include "Fence.hpp"

Fence::Fence(Device *device) {
    this->device = device;

    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fenceInfo.pNext = NULL;

    this->fence = new VkFence();

    if (vkCreateFence(device->getDevice(), &fenceInfo, nullptr, fence) !=
        VK_SUCCESS) {
        throw std::runtime_error(
            "failed to create synchronization objects for a frame!");
    }
}

VkFence *Fence::getFence() { return fence; }

VkResult Fence::getStatus() {
    return vkGetFenceStatus(device->getDevice(), *fence);
}

void Fence::reset() { vkResetFences(device->getDevice(), 1, fence); }

void Fence::wait() {
    vkWaitForFences(device->getDevice(), 1, fence, VK_TRUE, UINT64_MAX);
}

Fence::~Fence() {
    vkDestroyFence(device->getDevice(), *fence, nullptr);
    delete fence;
}