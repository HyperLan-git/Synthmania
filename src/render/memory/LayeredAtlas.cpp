#include "LayeredAtlas.hpp"

LayeredAtlas::LayeredAtlas(std::shared_ptr<Image> image)
    : img(image), device(image->getDevice()), layer(0) {}

bool LayeredAtlas::fits(Image &image) const {
    VkExtent3D e1 = this->img->getExtent(), e2 = image.getExtent();
    if (e2.width > e1.width || e2.height > e1.height) return false;
    return layer < this->img->getLayers();
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

TexPtr LayeredAtlas::getTexture(uint32_t layer, VkFormat format,
                                VkImageAspectFlags aspectFlags,
                                std::string name) {
    return std::make_shared<ImageView>(this->img, format, aspectFlags, name,
                                       layer);
}

LayeredAtlas::~LayeredAtlas() {}
