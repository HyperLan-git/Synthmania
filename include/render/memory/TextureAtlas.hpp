#pragma once

class TextureAtlas;

#include "CommandBuffer.hpp"

class TextureAtlas {
   public:
    TextureAtlas(Device *device, Image *image, VkFormat format,
                 VkImageAspectFlags aspectFlags, std::string name);

    /**
     * Checks whether a new image can fit in this atlas
     * @return false if the atlas does not have enough remaining space
     */
    bool fits(ImageView *image) const;

    /**
     * Adds to the atlas another image and creates a new view
     * @arg{ImageView *imageToCopy} an image view
     * @throw std::runtime_error if there is not enough space inside the atlas,
     * make a new one instead and check with TextureAtlas::fits !
     * @return The index of the texture in the atlas
     */
    glm::vec<4, uint32_t> append(ImageView *imageToCopy, CommandPool *pool);

    bool contains(std::string texture) const;

    glm::vec<4, uint32_t> operator[](std::string texture) const;

    ImageView *getTexture();

    virtual ~TextureAtlas();

   private:
    Device *device;
    ImageView *img;
    // Write pointer to the upper-left corner of where we can add a new image
    glm::vec2 ptr;
    // Height of the current write row
    float height;
    std::map<std::string, glm::vec<4, uint32_t>> contents;
};