#pragma once

class ImageView;

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <string>

#include "Image.hpp"

class ImageView {
   public:
    ImageView(Device *device, Image *image, VkFormat format,
              VkImageAspectFlags aspectFlags, std::string name);
    VkImageView *getView();
    Image *getImage();
    std::string getName();
    ~ImageView();

   private:
    Device *device;
    VkImageView *view;
    Image *image;
    std::string name;
};