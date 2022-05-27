#pragma once

class Image;

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "Memory.hpp"

class Image {
   public:
    Image(VkPhysicalDevice *physicalDevice, Device *device, uint32_t width,
          uint32_t height, VkFormat format, VkImageTiling tiling,
          VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    Image(Device *device, VkImage *image, VkExtent2D extent);
    VkImage *getImage();
    Memory *getMemory();
    VkExtent3D getExtent();
    ~Image();

   private:
    Device *device;
    VkImage *image;
    Memory *memory;
    VkExtent3D extent;
};

std::vector<Image *> createImagesForSwapchain(Device *device,
                                              VkSwapchainKHR swapChain,
                                              uint32_t *imageCount,
                                              VkExtent2D extent);