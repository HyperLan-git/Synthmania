#include "ComputeShader.hpp"

ComputeShader::ComputeShader(Device& device, const std::vector<char>& code,
                             const char* mainFunction, uint64_t workers)
    : device(device) {
    VkShaderModuleCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    createInfo.pNext = NULL;
    createInfo.flags = 0;

    if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr,
                             &module) != VK_SUCCESS)
        throw std::runtime_error("failed to create compute shader module!");

    VkPipelineShaderStageCreateInfo shaderStageInfo;
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = module;
    shaderStageInfo.pName = mainFunction;
    shaderStageInfo.flags = 0;
    shaderStageInfo.pSpecializationInfo = NULL;
    shaderStageInfo.pNext = NULL;

    this->info = shaderStageInfo;
    this->workers = workers;
}

VkShaderModule ComputeShader::getModule() { return module; }

VkPipelineShaderStageCreateInfo ComputeShader::toPipeline() { return info; }

uint64_t ComputeShader::getWorkers() { return workers; }

Device& ComputeShader::getDevice() { return device; }

ComputeShader::~ComputeShader() {
    vkDestroyShaderModule(this->device.getDevice(), this->module, NULL);
}