#pragma once

#include <stdexcept>

#include "Device.hpp"

class Fence {
   public:
    Fence(Device &device);

    Fence(Fence &&) = delete;
    Fence &operator=(Fence &&) = delete;

    Fence(const Fence &) = delete;
    Fence &operator=(const Fence &) = delete;

    VkFence getFence();

    VkResult getStatus();

    void reset();
    void wait();

    ~Fence();

   private:
    Device &device;
    VkFence fence;
};