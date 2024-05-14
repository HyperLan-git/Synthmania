#pragma once

class Swapchain;

#include "RenderPass.hpp"

class Swapchain {
   public:
    Swapchain(Device &device, Window &window, VkSurfaceKHR surface);

    Swapchain(Swapchain &&) = delete;
    Swapchain &operator=(Swapchain &&) = delete;

    Swapchain(const Swapchain &) = delete;
    Swapchain &operator=(const Swapchain &) = delete;

    RenderPass *getRenderPass();
    std::vector<Framebuffer *> getFramebuffers();
    VkExtent2D getExtent();
    VkSwapchainKHR getSwapchain();

    ~Swapchain();

   private:
    Device &device;
    VkSwapchainKHR swapchain;
    VkFormat imageFormat;
    VkExtent2D extent;

    std::vector<std::shared_ptr<Image>> images;
    std::vector<TexPtr> imageViews;
    std::vector<Framebuffer *> framebuffers;

    std::shared_ptr<Image> depthImage;
    TexPtr depthImageView;

    RenderPass *renderPass;
};