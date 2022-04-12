#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Device.hpp"

class Semaphore {
   public:
    Semaphore(Device *device);

    VkSemaphore *getSemaphore();

    ~Semaphore();

   private:
    VkSemaphore *semaphore;
    Device *device;
};