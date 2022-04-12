#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Device.hpp"

class Fence {
   public:
    Fence(Device *device);

    VkFence *getFence();

    void reset();
    void wait();

    ~Fence();

   private:
    Device *device;
    VkFence *fence;
};