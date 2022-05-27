#pragma once

class RenderPass;

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Device.hpp"
#include "Utils.hpp"

class RenderPass {
   public:
    RenderPass(VkPhysicalDevice *physicalDevice, Device *device,
               VkFormat swapChainImageFormat);

    VkRenderPass *getPass();

    ~RenderPass();

   private:
    Device *device;
    VkRenderPass *pass;
};