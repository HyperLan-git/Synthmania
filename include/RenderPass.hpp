#pragma once

class RenderPass;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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