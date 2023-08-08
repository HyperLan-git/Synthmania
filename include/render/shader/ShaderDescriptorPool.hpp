#pragma once

class ShaderDescriptorPool;

#include <stdexcept>

#include "Device.hpp"

// TODO count what's left to allocate
class ShaderDescriptorPool {
   public:
    ShaderDescriptorPool(Device &device, VkDescriptorType *types,
                         uint32_t count);
    ShaderDescriptorPool(Device &device, VkDescriptorType *types,
                         uint32_t *typeCounts, uint32_t count);

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