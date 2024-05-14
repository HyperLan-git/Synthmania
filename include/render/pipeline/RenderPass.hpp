#pragma once

class RenderPass;

#include "Utils.hpp"

class RenderPass {
   public:
    RenderPass(Device &device, VkFormat imageFormat,
               VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    RenderPass(RenderPass &&) = delete;
    RenderPass &operator=(RenderPass &&) = delete;

    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &) = delete;

    VkRenderPass getPass();
    Device &getDevice();

    ~RenderPass();

   private:
    Device &device;
    VkRenderPass pass;
};