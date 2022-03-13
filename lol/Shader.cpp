#include "Shader.hpp"

Shader::Shader(const char* mainFunction, VkDevice* device,
               const std::vector<char>& code, bool vertex) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule* shaderModule = new VkShaderModule();
    if (vkCreateShaderModule(*device, &createInfo, nullptr, shaderModule) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    VkPipelineShaderStageCreateInfo shaderStageInfo =
        VkPipelineShaderStageCreateInfo();
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage =
        vertex ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageInfo.module = VkShaderModule(*shaderModule);
    shaderStageInfo.pName = mainFunction;

    this->info = shaderStageInfo;
    this->vertex = vertex;
    this->module = shaderModule;
    this->device = device;
}

VkPipelineShaderStageCreateInfo Shader::toPipeline() { return info; }

Shader::~Shader() {
    vkDestroyShaderModule(*(this->device), *(this->module), nullptr);
    delete module;
}