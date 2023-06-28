#pragma once

#include <stdexcept>

#include "Device.hpp"

class CommandPool {
   public:
    CommandPool(Device* device);
    VkCommandPool* getPool();
    ~CommandPool();

   private:
    Device* device;
    VkCommandPool* pool;
};