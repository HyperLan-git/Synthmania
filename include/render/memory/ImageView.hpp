#pragma once

class ImageView;

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Image.hpp"

class ImageView {
   public:
    ImageView(Device *device, Image *image, VkFormat format,
              VkImageAspectFlags aspectFlags, const char *name);
    VkImageView *getView();
    Image *getImage();
    const char *getName();
    ~ImageView();

   private:
    Device *device;
    VkImageView *view;
    Image *image;
    const char *name;
};