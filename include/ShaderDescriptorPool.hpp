#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <stdexcept>

#include "Device.hpp"

class ShaderDescriptorPool {
   public:
    ShaderDescriptorPool(Device *device, VkDescriptorType *types,
                         uint32_t count);

    VkDescriptorPool *getPool();

    ~ShaderDescriptorPool();

   private:
    Device *device;
    VkDescriptorPool *pool;
};