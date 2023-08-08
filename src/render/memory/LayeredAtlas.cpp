#include "LayeredAtlas.hpp"

LayeredAtlas::LayeredAtlas(std::shared_ptr<Image> image, VkFormat format,
                           VkImageAspectFlags aspectFlags, std::string name)
    : img(image), device(image->getDevice()), layer(0), contents() {
    this->views.reserve(image->getLayers());
    for (uint32_t l = 0; l < image->getLayers(); l++)
        this->views.emplace_back(image, format, aspectFlags, name, l);
}

bool LayeredAtlas::fits(ImageView *image) const {
    return layer < image->getImage()->getLayers();
}

uint32_t LayeredAtlas::append(Image &imageToCopy, CommandPool &pool) {
    VkExtent3D ext = imageToCopy.getExtent();
    CommandBuffer buf(pool, true);
    if (buf.begin()) return -1;
    buf.copyImage(imageToCopy, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *this->img,
                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {0, 0, 0}, {0, 0, 0},
                  ext, layer);
    if (buf.end()) return -1;
    if (buf.submit(*device.getQueue("secondary"))) return -1;
    return layer++;
}

Image &LayeredAtlas::getImage() { return *this->img; }

ImageView &LayeredAtlas::getTexture(uint32_t layer) {
    return this->views[layer];
}

LayeredAtlas::~LayeredAtlas() {}
