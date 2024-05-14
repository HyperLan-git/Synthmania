#include "TextureAtlas.hpp"

TextureAtlas::TextureAtlas(std::shared_ptr<Image> image, VkFormat format,
                           VkImageAspectFlags aspectFlags, std::string name)
    : img(std::make_shared<ImageView>(image, format, aspectFlags, name)),
      device(image->getDevice()),
      ptr(0, 0),
      height(0),
      contents() {}

bool TextureAtlas::fits(VkExtent2D &imageExtent) const {
    VkExtent3D thisExt = this->img->getImage().getExtent();
    bool fitsHorizontally = imageExtent.width <= thisExt.width - this->ptr.x &&
                            imageExtent.height <= thisExt.height - this->ptr.y;
    if (fitsHorizontally) return true;
    return imageExtent.width <= thisExt.width &&
           imageExtent.height + height + this->ptr.y <= thisExt.height;
}

glm::vec<4, uint32_t> TextureAtlas::append(Image &imageToCopy, Texture id,
                                           CommandPool &pool) {
    VkExtent3D imgExt = imageToCopy.getExtent(),
               thisExt = this->img->getImage().getExtent();
    bool fitsHorizontally = imgExt.width <= thisExt.width - this->ptr.x &&
                            imgExt.height <= thisExt.height - this->ptr.y;
    if (fitsHorizontally) {
        if (imgExt.height > this->height) this->height = imgExt.height;
    } else {
        if (imgExt.width > thisExt.width ||
            imgExt.height + height + this->ptr.y > thisExt.height)
            throw std::runtime_error("Cannot append image to atlas!");
        this->ptr.x = 0;
        this->ptr.y = this->ptr.y + this->height;
        this->height = imgExt.height;
    }
    glm::vec<4, uint32_t> ext{this->ptr.x, this->ptr.y, imgExt.width,
                              imgExt.height};
    CommandBuffer buf(pool, true);
    buf.begin();
    buf.copyImage(imageToCopy, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                  this->img->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                  {0, 0, 0}, {(int32_t)ext.x, (int32_t)ext.y, 1}, imgExt);
    buf.end();
    buf.submit(*device.getQueue("secondary"));
    this->ptr.x = this->ptr.x + imgExt.width;
    this->contents.emplace(id, ext);
    return ext;
}

bool TextureAtlas::contains(Texture texture) const {
    return this->contents.find(texture) != this->contents.cend();
}

glm::vec<4, uint32_t> TextureAtlas::operator[](Texture name) const {
    return this->contents.at(name);
}

TexPtr TextureAtlas::getTexture() { return this->img; }

Device &TextureAtlas::getDevice() { return this->device; }

TextureAtlas::~TextureAtlas() {}