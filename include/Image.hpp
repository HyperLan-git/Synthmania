#pragma once

class Image;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "Memory.hpp"

class Image {
   public:
    Image(VkPhysicalDevice *physicalDevice, Device *device, uint32_t width,
          uint32_t height, VkFormat format, VkImageTiling tiling,
          VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    Image(Device *device, VkImage *image);
    VkImage *getImage();
    Memory *getMemory();
    ~Image();

   private:
    Device *device;
    VkImage *image;
    Memory *memory;
};

std::vector<Image *> createImagesForSwapchain(Device *device,
                                              VkSwapchainKHR swapChain,
                                              uint32_t *imageCount);