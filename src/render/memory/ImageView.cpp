#include "ImageView.hpp"

ImageView::ImageView(Device *device, Image *image, VkFormat format,
                     VkImageAspectFlags aspectFlags) {
    this->device = device;
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = *(image->getImage());
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    view = new VkImageView();
    if (vkCreateImageView(*(device->getDevice()), &viewInfo, nullptr, view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

VkImageView *ImageView::getView() { return view; }

ImageView::~ImageView() {
    vkDestroyImageView(*(device->getDevice()), *view, nullptr);
    delete view;
}