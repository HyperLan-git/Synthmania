#pragma once

class ShaderDescriptorPool;

#include <stdexcept>

#include "Device.hpp"

class ShaderDescriptorPool {
   public:
    ShaderDescriptorPool(Device *device, VkDescriptorType *types,
                         uint32_t count);
    ShaderDescriptorPool(Device *device, VkDescriptorType *types,
                         uint32_t *typeCounts, uint32_t count);

    VkDescriptorPool *getPool();

    ~ShaderDescriptorPool();

   private:
    Device *device;
    VkDescriptorPool *pool;
};