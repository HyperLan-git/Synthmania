#include "Fence.hpp"

Fence::Fence(VkDevice *device) {
    this->device = device;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    this->fence = new VkFence();

    if (vkCreateFence(*device, &fenceInfo, nullptr, fence) != VK_SUCCESS) {
        throw std::runtime_error(
            "failed to create synchronization objects for a frame!");
    }
}

VkFence *Fence::getFence() { return fence; }

void Fence::reset() { vkResetFences(*device, 1, fence); }

void Fence::wait() { vkWaitForFences(*device, 1, fence, VK_TRUE, UINT64_MAX); }

Fence::~Fence() {
    vkDestroyFence(*device, *fence, nullptr);
    delete fence;
}