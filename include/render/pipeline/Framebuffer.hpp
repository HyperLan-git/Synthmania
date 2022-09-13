#pragma once

class Framebuffer;

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "Device.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"

class Framebuffer {
   public:
    Framebuffer(Device *device, RenderPass *pass, VkExtent2D extent,
                std::vector<ImageView *> views);
    VkFramebuffer *getFramebuffer();

    VkExtent2D getExtent();

    ~Framebuffer();

   private:
    Device *device;
    VkFramebuffer *framebuffer;
    VkExtent2D extent;
};