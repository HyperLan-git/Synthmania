#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <stdexcept>

class ShaderDescriptorPool {
   public:
    ShaderDescriptorPool(VkDevice *device, VkDescriptorType *types,
                         uint32_t count);

    VkDescriptorPool *getPool();

    ~ShaderDescriptorPool();

   private:
    VkDevice *device;
    VkDescriptorPool *pool;
};