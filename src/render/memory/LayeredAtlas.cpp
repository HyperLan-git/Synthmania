#include "LayeredAtlas.hpp"

LayeredAtlas::LayeredAtlas(Device *device, Image *image, VkFormat format,
                           VkImageAspectFlags aspectFlags, std::string name)
    : img(image), device(device), layer(0), contents() {
    for (uint32_t l = 0; l < image->getLayers(); l++) {
        this->views.push_back(
            new ImageView(device, image, format, aspectFlags, name, l));
    }
}

bool LayeredAtlas::fits(ImageView *image) const {
    return layer < image->getImage()->getLayers();
}

uint32_t LayeredAtlas::append(Image *imageToCopy, CommandPool *pool) {
    VkExtent3D ext = imageToCopy->getExtent();
    CommandBuffer buf(device, pool, true);
    buf.begin();
    buf.copyImage(imageToCopy, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, this->img,
                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {0, 0, 0}, {0, 0, 0},
                  ext, layer);
    buf.end();
    buf.submit(device->getQueue("secondary"));
    return layer++;
}

Image *LayeredAtlas::getImage() { return this->img; }

ImageView *LayeredAtlas::getTexture(uint32_t layer) {
    return this->views[layer];
}

LayeredAtlas::~LayeredAtlas() { delete this->img; }
