#pragma once

class ImageView;
#include <memory>
typedef std::shared_ptr<ImageView> TexPtr;

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
    Image &getImage();
    std::string getName() const;
    Device &getDevice();
    ~ImageView();

   private:
    Device &device;
    VkImageView view;
    std::shared_ptr<Image> image;
    std::string name;
};