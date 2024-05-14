#pragma once

class ShaderDescriptorPool;

#include <stdexcept>

#include "Device.hpp"

// TODO count what's left to allocate
class ShaderDescriptorPool {
   public:
    ShaderDescriptorPool(Device &device,
                         std::initializer_list<VkDescriptorPoolSize> types);
    ShaderDescriptorPool(Device &device,
                         const std::vector<VkDescriptorPoolSize> &types);

    ShaderDescriptorPool(ShaderDescriptorPool &&) = delete;
    ShaderDescriptorPool &operator=(ShaderDescriptorPool &&) = delete;

    ShaderDescriptorPool(const ShaderDescriptorPool &) = delete;
    ShaderDescriptorPool &operator=(const ShaderDescriptorPool &) = delete;

    VkDescriptorPool getPool();
    Device &getDevice();

    ~ShaderDescriptorPool();

   private:
    Device &device;
    VkDescriptorPool pool;
};