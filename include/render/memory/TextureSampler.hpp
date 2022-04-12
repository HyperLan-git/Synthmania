#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Device.hpp"

class TextureSampler {
   public:
    TextureSampler(VkPhysicalDevice* physicalDevice, Device* device);

    VkSampler* getSampler();

    ~TextureSampler();

   private:
    Device* device;
    VkSampler* sampler;
};