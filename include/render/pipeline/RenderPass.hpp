#pragma once

class RenderPass;

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Device.hpp"
#include "Utils.hpp"

class RenderPass {
   public:
    RenderPass(VkPhysicalDevice *physicalDevice, Device *device,
               VkFormat imageFormat,
               VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VkRenderPass *getPass();

    ~RenderPass();

   private:
    Device *device;
    VkRenderPass *pass;
};