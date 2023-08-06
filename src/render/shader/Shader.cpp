#include "Shader.hpp"

Shader::Shader(const char* mainFunction, Device* device,
               const std::vector<char>& code, VkShaderStageFlagBits type) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule* shaderModule = new VkShaderModule();
    if (vkCreateShaderModule(device->getDevice(), &createInfo, nullptr,
                             shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = type;
    shaderStageInfo.module = *shaderModule;
    shaderStageInfo.pName = mainFunction;

    this->info = shaderStageInfo;
    this->type = type;
    this->module = shaderModule;
    this->device = device;
}

VkPipelineShaderStageCreateInfo Shader::toPipeline() { return info; }

VkShaderModule* Shader::getModule() { return module; }

Shader::~Shader() {
    vkDestroyShaderModule(this->device->getDevice(), *(this->module), NULL);
    delete module;
}