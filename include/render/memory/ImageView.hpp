#pragma once

class ImageView;

#include "Image.hpp"

class ImageView {
   public:
    ImageView(Device *device, Image *image, VkFormat format,
              VkImageAspectFlags aspectFlags, std::string name,
              uint32_t layer = 0);

    ImageView(ImageView &&img);
    ImageView &operator=(ImageView &&img);

    ImageView(const ImageView &img) = delete;
    ImageView &operator=(const ImageView &img) = delete;

    VkImageView getView();
    Image *getImage();
    std::string getName();
    ~ImageView();

   private:
    Device *device;
    VkImageView view;
    Image *image;
    std::string name;
};