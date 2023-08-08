#pragma once

class ImageView;

#include "Image.hpp"

class ImageView {
   public:
    ImageView(std::shared_ptr<Image> image, VkFormat format,
              VkImageAspectFlags aspectFlags, std::string name,
              uint32_t layer = 0);

    ImageView(ImageView &&img);
    ImageView &operator=(ImageView &&img);

    ImageView(const ImageView &img) = delete;
    ImageView &operator=(const ImageView &img) = delete;

    VkImageView getView();
    std::shared_ptr<Image> &getImage();
    std::string getName();
    Device &getDevice();
    ~ImageView();

   private:
    Device &device;
    VkImageView view;
    std::shared_ptr<Image> image;
    std::string name;
};