#pragma once

class Framebuffer;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "ImageView.hpp"

class Framebuffer {
   public:
    Framebuffer(VkDevice *device, VkRenderPass pass, VkExtent2D extent,
                std::vector<ImageView *> views);
    VkFramebuffer *getFramebuffer();
    ~Framebuffer();

   private:
    VkDevice *device;
    VkFramebuffer *framebuffer;
};