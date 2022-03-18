#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

class Fence {
   public:
    Fence(VkDevice *device);

    VkFence *getFence();

    void reset();
    void wait();

    ~Fence();

   private:
    VkDevice *device;
    VkFence *fence;
};