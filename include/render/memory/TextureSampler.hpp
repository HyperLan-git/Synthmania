#pragma once

#include "Device.hpp"
#include "ImageView.hpp"

class TextureSampler {
   public:
    TextureSampler(VkPhysicalDevice* physicalDevice, Device* device);

    VkSampler* getSampler();

    VkDescriptorImageInfo* createImageInfo(ImageView* view);

    virtual ~TextureSampler();

   private:
    Device* device;
    VkSampler* sampler;
};