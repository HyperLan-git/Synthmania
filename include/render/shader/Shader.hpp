#pragma once
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "Device.hpp"

class Shader {
   public:
    Shader(const char* mainFunction, Device* device,
           const std::vector<char>& code, bool vertex);

    VkPipelineShaderStageCreateInfo toPipeline();

    ~Shader();

   private:
    Device* device;
    VkShaderModule* module;
    const char* mainFunction;
    bool vertex;
    VkPipelineShaderStageCreateInfo info;
};