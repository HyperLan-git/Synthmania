#pragma once

class ComputeShader;

#include <stdexcept>

#include "Device.hpp"

class ComputeShader {
   public:
    ComputeShader(Device* device, const std::vector<char>& code,
                  const char* mainFunction, uint64_t workers);

    VkShaderModule* getModule();

    VkPipelineShaderStageCreateInfo toPipeline();

    uint64_t getWorkers();

    ~ComputeShader();

   private:
    Device* device;
    VkPipelineShaderStageCreateInfo info;
    VkShaderModule* module;
    uint64_t workers;
};
