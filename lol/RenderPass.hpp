#pragma once

class RenderPass;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Utils.hpp"

class RenderPass {
   public:
    RenderPass(VkPhysicalDevice *physicalDevice, VkDevice *device,
               VkFormat swapChainImageFormat);

    VkRenderPass *getPass();

    ~RenderPass();

   private:
    VkDevice *device;
    VkRenderPass *pass;
};