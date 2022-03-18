#pragma once

class PipelineLayout;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "ShaderDescriptorSetLayout.hpp"

class PipelineLayout {
   public:
    PipelineLayout(VkDevice* device, ShaderDescriptorSetLayout* shaderLayout);
    VkPipelineLayout* getLayout();
    ~PipelineLayout();

   private:
    VkDevice* device;
    VkPipelineLayout* layout;
};