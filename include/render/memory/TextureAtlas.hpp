#pragma once

class TextureAtlas;

#include "CommandBuffer.hpp"

class TextureAtlas {
   public:
    TextureAtlas(std::shared_ptr<Image> image, VkFormat format,
                 VkImageAspectFlags aspectFlags, std::string name);

    TextureAtlas(TextureAtlas &&) = delete;
    TextureAtlas &operator=(TextureAtlas &&) = delete;

    TextureAtlas(const TextureAtlas &) = delete;
    TextureAtlas &operator=(const TextureAtlas &) = delete;

    /**
     * Checks whether a new image can fit in this atlas
     * @return false if the atlas doesn't have enough space and true otherwise
     */
    bool fits(VkExtent2D &imageExtent) const;

    /**
     * Adds to the atlas another image
     * @arg{Image &imageToCopy} an image
     * @arg{Texture id} the id of the texture to index it
     * @arg{CommandPool &pool} a command pool used for a temporary CommandBuffer
     * @throw std::runtime_error if there is not enough space inside the atlas,
     * make a new one instead and check with TextureAtlas::fits !
     * @return The position and size of the texture in the atlas
     */
    glm::vec<4, uint32_t> append(Image &imageToCopy, Texture id,
                                 CommandPool &pool);

    bool contains(Texture texture) const;

    glm::vec<4, uint32_t> operator[](Texture texture) const;

    TexPtr getTexture();

    Device &getDevice();

    virtual ~TextureAtlas();

   private:
    Device &device;
    std::shared_ptr<ImageView> img;

    // Write pointer to the upper-left corner of where we can add a new image
    glm::vec2 ptr;
    // Height of the current write row
    float height;
    std::map<Texture, glm::vec<4, uint32_t>> contents;
};