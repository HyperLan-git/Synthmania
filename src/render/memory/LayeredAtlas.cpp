#include "LayeredAtlas.hpp"

LayeredAtlas::LayeredAtlas(Device *device, Image *image, VkFormat format,
                           VkImageAspectFlags aspectFlags, std::string name)
    : img(new ImageView(device, image, format, aspectFlags, name)),
      device(device),
      layer(0),
      contents() {}

bool LayeredAtlas::fits(ImageView *image) const {
    return layer < img->getImage()->getLayers();
}

uint32_t LayeredAtlas::append(ImageView *imageToCopy, CommandPool *pool) {
    VkExtent3D ext = imageToCopy->getImage()->getExtent();
    CommandBuffer buf(device, pool, true);
    buf.begin();
    buf.copyImage(imageToCopy->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                  this->img->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                  {0, 0, 0}, {0, 0, 0}, ext, layer);
    buf.end();
    buf.submit(device->getQueue("secondary"));
    return layer++;
}

ImageView *LayeredAtlas::getTexture() { return this->img; }

LayeredAtlas::~LayeredAtlas() { delete this->img; }
