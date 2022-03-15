#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

class TextureSampler {
   public:
    TextureSampler(VkPhysicalDevice* physicalDevice, VkDevice* device);

    VkSampler* getSampler();

    ~TextureSampler();

   private:
    VkDevice* device;
    VkSampler* sampler;
};