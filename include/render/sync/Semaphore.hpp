#pragma once

#include <vulkan/vulkan.h>

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