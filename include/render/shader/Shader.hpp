#pragma once

class Shader;

#include <stdexcept>

#include "Device.hpp"

class Shader {
   public:
    Shader(const char* mainFunction, Device& device,
           const std::vector<char>& code, VkShaderStageFlagBits type);

    Shader(Shader&&);
    Shader& operator=(Shader&&);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    VkPipelineShaderStageCreateInfo toPipeline();
    VkShaderModule* getModule();

    ~Shader();

   private:
    Device& device;
    VkShaderModule* module;
    const char* mainFunction;
    VkShaderStageFlagBits type;
    VkPipelineShaderStageCreateInfo info;
};