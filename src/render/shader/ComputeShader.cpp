#include "ComputeShader.hpp"

ComputeShader::ComputeShader(Device* device, const std::vector<char>& code,
                             const char* mainFunction, uint64_t workers) {
    VkShaderModuleCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    createInfo.pNext = NULL;
    createInfo.flags = 0;

    VkShaderModule* shaderModule = new VkShaderModule();
    if (vkCreateShaderModule(*(device->getDevice()), &createInfo, nullptr,
                             shaderModule) != VK_SUCCESS)
        throw std::runtime_error("failed to create compute shader module!");

    VkPipelineShaderStageCreateInfo shaderStageInfo =
        VkPipelineShaderStageCreateInfo();
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = *shaderModule;
    shaderStageInfo.pName = mainFunction;

    this->info = shaderStageInfo;
    this->module = shaderModule;
    this->device = device;
    this->workers = workers;
}

VkShaderModule* ComputeShader::getModule() { return module; }

VkPipelineShaderStageCreateInfo ComputeShader::toPipeline() { return info; }

uint64_t ComputeShader::getWorkers() { return workers; }

ComputeShader::~ComputeShader() {
    vkDestroyShaderModule(*(this->device->getDevice()), *(this->module), NULL);
    delete module;
}