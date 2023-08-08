#include "PipelineLayout.hpp"

PipelineLayout::PipelineLayout(
    ShaderDescriptorSetLayout& shaderLayout,
    std::initializer_list<VkPushConstantRange> constantRanges)
    : device(shaderLayout.getDevice()) {
    VkDescriptorSetLayout l = shaderLayout.getLayout();
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &l;
    pipelineLayoutInfo.pushConstantRangeCount = constantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges = constantRanges.begin();

    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr,
                               &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

VkPipelineLayout PipelineLayout::getLayout() { return layout; }

Device& PipelineLayout::getDevice() { return device; }

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device.getDevice(), layout, nullptr);
}