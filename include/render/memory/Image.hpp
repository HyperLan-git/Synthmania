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

    Image(const Image &img) = delete;
    Image &operator=(const Image &img) = delete;

    VkImage *getImage();
    /**
     * @brief Get the Memory associated with this image.
     *
     * A memory pointer that is NULL indicates an image from a swapchain with no
     * memory associated.
     *
     * @return Memory*
     */
    Memory *getMemory();
    VkExtent3D getExtent();
    VkSubresourceLayout getImageSubresourceLayout(
        uint32_t mipLevel = 0, uint32_t arrayLayer = 0,
        VkImageAspectFlags flags = VK_IMAGE_ASPECT_COLOR_BIT);
    void write(const void *data, VkDeviceSize sz, VkDeviceSize offset);
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