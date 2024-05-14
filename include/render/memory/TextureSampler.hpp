#pragma once

#include "Device.hpp"
#include "ImageView.hpp"

class TextureSampler {
   public:
    TextureSampler(Device& device);

    TextureSampler(TextureSampler&& sampler);
    TextureSampler& operator=(TextureSampler&& sampler);

    TextureSampler(const TextureSampler& sampler) = delete;
    TextureSampler& operator=(const TextureSampler& sampler) = delete;

    VkSampler getSampler();

    VkDescriptorImageInfo createImageInfo(ImageView& view);

    virtual ~TextureSampler();

   private:
    Device& device;
    VkSampler sampler;
};