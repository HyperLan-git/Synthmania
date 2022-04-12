#pragma once

class PipelineLayout;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Device.hpp"
#include "ShaderDescriptorSetLayout.hpp"

class PipelineLayout {
   public:
    PipelineLayout(Device* device, ShaderDescriptorSetLayout* shaderLayout,
                   uint32_t constantRangeCount,
                   VkPushConstantRange* constantRanges);
    VkPipelineLayout* getLayout();
    ~PipelineLayout();

   private:
    Device* device;
    VkPipelineLayout* layout;
};