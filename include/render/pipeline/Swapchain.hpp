#pragma once

class Swapchain;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
              Shader *vertShader, Shader *fragShader,
              ShaderDescriptorSetLayout *shaderDescriptors,
              VkSurfaceKHR *surface, VkPushConstantRange *ranges,
              uint32_t rangeCount);

    RenderPass *getRenderPass();
    std::vector<Framebuffer *> getFramebuffers();
    VkExtent2D getExtent();
    Pipeline *getPipeline();
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

    PipelineLayout *pipelineLayout;
    Pipeline *graphicsPipeline;
};