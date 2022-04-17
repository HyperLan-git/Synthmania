#pragma once

class ImageView;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Image.hpp"

class ImageView {
   public:
    ImageView(Device *device, Image *image, VkFormat format,
              VkImageAspectFlags aspectFlags);
    VkImageView *getView();
    Image *getImage();
    ~ImageView();

   private:
    Device *device;
    VkImageView *view;
    Image *image;
};