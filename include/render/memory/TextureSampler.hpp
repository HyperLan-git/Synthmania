#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Device.hpp"

class TextureSampler {
   public:
    TextureSampler(VkPhysicalDevice* physicalDevice, Device* device);

    VkSampler* getSampler();

    virtual ~TextureSampler();

   private:
    Device* device;
    VkSampler* sampler;
};