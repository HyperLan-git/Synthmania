#pragma once

class Framebuffer;

#include "ImageView.hpp"
#include "RenderPass.hpp"

class Framebuffer {
   public:
    Framebuffer(RenderPass &pass, VkExtent2D extent,
                std::vector<ImageView *> views);

    Framebuffer(Framebuffer &&) = delete;
    Framebuffer &operator=(Framebuffer &&) = delete;

    VkFramebuffer getFramebuffer();

    VkExtent2D getExtent();

    Device &getDevice();
    RenderPass &getRenderPass();

    ~Framebuffer();

   private:
    Device &device;
    RenderPass &pass;
    VkFramebuffer framebuffer;
    VkExtent2D extent;
};