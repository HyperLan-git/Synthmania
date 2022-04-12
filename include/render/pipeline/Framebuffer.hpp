#pragma once

class Framebuffer;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    ~Framebuffer();

   private:
    Device *device;
    VkFramebuffer *framebuffer;
};