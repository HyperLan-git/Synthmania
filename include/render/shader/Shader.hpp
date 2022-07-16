#pragma once
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "Device.hpp"

class Shader {
   public:
    Shader(const char* mainFunction, Device* device,
           const std::vector<char>& code, VkShaderStageFlagBits type);

    VkPipelineShaderStageCreateInfo toPipeline();
    VkShaderModule* getModule();

    ~Shader();

   private:
    Device* device;
    VkShaderModule* module;
    const char* mainFunction;
    VkShaderStageFlagBits type;
    VkPipelineShaderStageCreateInfo info;
};