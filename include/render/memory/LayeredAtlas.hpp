#pragma once

class LayeredAtlas;

#include "CommandBuffer.hpp"

class LayeredAtlas {
   public:
    LayeredAtlas(Device *device, Image *image, VkFormat format,
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
     * make a new one instead and check with LayeredAtlas::fits !
     * @return The index of the texture in the atlas
     */
    uint32_t append(ImageView *imageToCopy, CommandPool *pool);

    ImageView *getTexture();

    virtual ~LayeredAtlas();

   private:
    Device *device;
    ImageView *img;
    // Write pointer
    uint32_t layer;
    std::map<std::string, uint32_t> contents;
};