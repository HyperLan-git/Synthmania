#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <stdexcept>
#include <vector>

#include "Device.hpp"

class CommandPool {
   public:
    CommandPool(VkPhysicalDevice physicalDevice, Device* device);
    VkCommandPool* getPool();
    ~CommandPool();

   private:
    Device* device;
    VkCommandPool* pool;
};