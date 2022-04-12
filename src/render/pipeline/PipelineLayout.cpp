#include "PipelineLayout.hpp"

PipelineLayout::PipelineLayout(Device* device,
                               ShaderDescriptorSetLayout* shaderLayout,
                               uint32_t constantRangeCount,
                               VkPushConstantRange* constantRanges) {
    this->device = device;
    this->layout = new VkPipelineLayout();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = shaderLayout->getLayout();
    pipelineLayoutInfo.pushConstantRangeCount = constantRangeCount;
    pipelineLayoutInfo.pPushConstantRanges = constantRanges;

    if (vkCreatePipelineLayout(*(device->getDevice()), &pipelineLayoutInfo,
                               nullptr, layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

VkPipelineLayout* PipelineLayout::getLayout() { return layout; }

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(*(device->getDevice()), *layout, nullptr);
    delete layout;
}