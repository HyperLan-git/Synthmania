#include "TextureAtlas.hpp"

TextureAtlas::TextureAtlas(Device *device, Image *image, VkFormat format,
                           VkImageAspectFlags aspectFlags, std::string name)
    : img(new ImageView(device, image, format, aspectFlags, name)),
      device(device),
      ptr(0, 0),
      height(0),
      contents() {}

bool TextureAtlas::fits(ImageView *image) const {
    VkExtent3D imgExt = image->getImage()->getExtent(),
               thisExt = this->img->getImage()->getExtent();
    bool fitsHorizontally = imgExt.width <= thisExt.width - this->ptr.x &&
                            imgExt.height <= thisExt.height - this->ptr.y;
    if (fitsHorizontally) return true;
    return imgExt.width <= thisExt.width &&
           imgExt.height <= thisExt.height - this->ptr.y - height;
}

glm::vec<4, uint32_t> TextureAtlas::append(ImageView *imageToCopy,
                                           CommandPool *pool) {
    VkExtent3D imgExt = imageToCopy->getImage()->getExtent(),
               thisExt = this->img->getImage()->getExtent();
    bool fitsHorizontally = imgExt.width <= thisExt.width - this->ptr.x &&
                            imgExt.height <= thisExt.height - this->ptr.y;
    if (fitsHorizontally) {
        if (imgExt.height > this->height) this->height = imgExt.height;
    } else {
        this->ptr.x = 0;
        this->ptr.y = this->ptr.y + this->height;
        this->height = imgExt.height;
    }
    glm::vec<4, uint32_t> ext{this->ptr.x, this->ptr.y, imgExt.width,
                              imgExt.height};
    CommandBuffer buf(device, pool, true);
    buf.begin();
    buf.copyImage(imageToCopy->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                  this->img->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                  {0, 0, 0}, {(int32_t)ext.x, (int32_t)ext.y, 1}, imgExt);
    buf.end();
    buf.submit(device->getQueue("secondary"));
    this->ptr.x = this->ptr.x + imgExt.width;
    this->contents.emplace(imageToCopy->getName(), ext);
    return ext;
}

bool TextureAtlas::contains(std::string texture) const {
    return this->contents.find(texture) != this->contents.cend();
}

glm::vec<4, uint32_t> TextureAtlas::operator[](std::string name) const {
    return this->contents.at(name);
}

ImageView *TextureAtlas::getTexture() { return this->img; }

TextureAtlas::~TextureAtlas() { delete this->img; }