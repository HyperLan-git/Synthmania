#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Device.hpp"

class Semaphore {
   public:
    Semaphore(Device *device);
    Semaphore(Device *device, uint64_t initialValue, uint64_t signal);

    VkSemaphore *getSemaphore();

    ~Semaphore();

   private:
    VkSemaphore *semaphore;
    Device *device;
};