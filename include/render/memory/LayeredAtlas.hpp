#pragma once

class LayeredAtlas;

#include <vector>

#include "CommandBuffer.hpp"

class LayeredAtlas {
   public:
    LayeredAtlas(std::shared_ptr<Image> image);

    LayeredAtlas(LayeredAtlas &&) = delete;
    LayeredAtlas &operator=(LayeredAtlas &&) = delete;

    LayeredAtlas(const LayeredAtlas &) = delete;
    LayeredAtlas &operator=(const LayeredAtlas &) = delete;

    /**
     * Checks whether a new image can fit in this atlas
     * @return false if the atlas does not have enough remaining space
     */
    bool fits(Image &image) const;

    /**
     * Adds to the atlas another image and creates a new view
     * @arg{Image *imageToCopy} an image
     * @throw std::runtime_error if there is not enough space inside the atlas,
     * make a new one instead and check with LayeredAtlas::fits !
     * @return The index of the texture in the atlas
     */
    uint32_t append(Image &imageToCopy, CommandPool &pool);

    TexPtr getTexture(uint32_t layer, VkFormat format,
                      VkImageAspectFlags aspectFlags, std::string name);

    Image &getImage();

    virtual ~LayeredAtlas();

   private:
    Device &device;
    std::shared_ptr<Image> img;
    // Write pointer
    uint32_t layer;
};
