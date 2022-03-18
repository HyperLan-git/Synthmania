#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

class Semaphore {
   public:
    Semaphore(VkDevice *device);

    VkSemaphore *getSemaphore();

    ~Semaphore();

   private:
    VkSemaphore *semaphore;
    VkDevice *device;
};