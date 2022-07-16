#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Device.hpp"

class Fence {
   public:
    Fence(Device *device);

    VkFence *getFence();

    VkResult getStatus();

    void reset();
    void wait();

    ~Fence();

   private:
    Device *device;
    VkFence *fence;
};