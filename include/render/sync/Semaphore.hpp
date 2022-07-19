#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Device.hpp"

class Semaphore {
   public:
    /**
     * @brief Creates a binary semaphore that can be waited on during command
     * buffer execution
     */
    Semaphore(Device *device);

    /**
     * @brief Constructs a timeline semaphore that may be used to synchronize
     * actions between multiple queues
     *
     * @param device
     * @param initialValue
     */
    Semaphore(Device *device, uint64_t initialValue);

    VkSemaphore *getSemaphore();
    VkSemaphoreType getType();

    ~Semaphore();

   private:
    VkSemaphore *semaphore;
    VkSemaphoreType type;
    Device *device;
};