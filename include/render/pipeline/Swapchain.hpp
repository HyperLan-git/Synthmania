#pragma once

class Swapchain;

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "Framebuffer.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "Window.hpp"

class Swapchain {
   public:
    Swapchain(Device *device, VkPhysicalDevice *physicalDevice, Window *window,
              VkSurfaceKHR *surface);

    RenderPass *getRenderPass();
    std::vector<Framebuffer *> getFramebuffers();
    VkExtent2D getExtent();
    VkSwapchainKHR *getSwapchain();

    ~Swapchain();

   private:
    Device *device;
    VkPhysicalDevice *physicalDevice;
    VkSwapchainKHR *swapchain;
    VkFormat imageFormat;
    VkExtent2D extent;

    std::vector<Image *> images;
    std::vector<ImageView *> imageViews;
    std::vector<Framebuffer *> framebuffers;

    Image *depthImage;
    ImageView *depthImageView;

    RenderPass *renderPass;
};