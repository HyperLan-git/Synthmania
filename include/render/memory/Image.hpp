#pragma once

class Image;

#include "Memory.hpp"

class Image {
   public:
    Image(Device &device, uint32_t width, uint32_t height, VkFormat format,
          VkImageTiling tiling, VkImageUsageFlags usage,
          VkMemoryPropertyFlags properties, uint32_t layers = 1);
    Image(Device &device, VkImage image, VkExtent2D extent);

    Image(Image &&img);
    Image &operator=(Image &&img);

    Image(const Image &img) = delete;
    Image &operator=(const Image &img) = delete;

    VkImage getImage();
    VkImageLayout getLayout();
    Device &getDevice();

    /**
     * @brief Get the Memory associated with this image.
     *
     * A memory pointer that is unassigned indicates an image from a swapchain
     * with no memory associated.
     *
     * @return std::unique_ptr<Memory>& a smart pointer to a memory object or
     * empty if this image is a swapchain image
     */
    std::unique_ptr<Memory> &getMemory();
    VkExtent3D getExtent() const;
    uint32_t getLayers() const;
    VkSubresourceLayout getImageSubresourceLayout(
        uint32_t mipLevel = 0, uint32_t arrayLayer = 0,
        VkImageAspectFlags flags = VK_IMAGE_ASPECT_COLOR_BIT);
    void write(const void *data, VkDeviceSize sz, VkDeviceSize offset);
    ~Image();

   private:
    Device &device;
    VkImage image;
    // TODO find how to make that layout shit work
    VkImageLayout currentLayout;
    std::unique_ptr<Memory> memory;
    VkExtent3D extent;
    uint32_t layers;
};

std::vector<std::shared_ptr<Image>> createImagesForSwapchain(
    Device &device, VkSwapchainKHR swapChain, VkExtent2D extent);