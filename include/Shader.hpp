#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

class Shader {
   public:
    Shader(const char* mainFunction, VkDevice* device,
           const std::vector<char>& code, bool vertex);

    VkPipelineShaderStageCreateInfo toPipeline();

    ~Shader();

   private:
    VkDevice* device;
    VkShaderModule* module;
    const char* mainFunction;
    bool vertex;
    VkPipelineShaderStageCreateInfo info;
};