#pragma once

#include "Device.hpp"

// XXX actually count shit in command pool
class CommandPool {
   public:
    CommandPool(Device &device);

    CommandPool(CommandPool &&) = delete;
    CommandPool &operator=(CommandPool &&) = delete;

    CommandPool(const CommandPool &) = delete;
    CommandPool &operator=(const CommandPool &) = delete;

    VkCommandPool getPool();
    Device &getDevice();
    ~CommandPool();

   private:
    Device &device;
    VkCommandPool pool;
};