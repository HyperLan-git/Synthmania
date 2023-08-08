#include "ImageView.hpp"

ImageView::ImageView(std::shared_ptr<Image> image, VkFormat format,
                     VkImageAspectFlags aspectFlags, std::string name,
                     uint32_t layer)
    : device(image->getDevice()), image(image) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image->getImage();
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = layer;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device.getDevice(), &viewInfo, nullptr, &view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
    this->name = name;
}

ImageView::ImageView(ImageView &&img) : device(img.device) {
    *this = std::move(img);
}

ImageView &ImageView::operator=(ImageView &&img) {
    assert(this->device == img.device);
    std::swap(this->view, img.view);
    std::swap(this->image, img.image);
    this->name = img.name;
    return *this;
}

VkImageView ImageView::getView() { return view; }

Image &ImageView::getImage() { return *image; }

std::string ImageView::getName() const { return name; }

Device &ImageView::getDevice() { return device; }

ImageView::~ImageView() {
    if (view) vkDestroyImageView(device.getDevice(), view, nullptr);
}