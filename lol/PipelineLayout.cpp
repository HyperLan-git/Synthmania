#include "PipelineLayout.hpp"

PipelineLayout::PipelineLayout(VkDevice* device,
                               ShaderDescriptorSetLayout* shaderLayout) {
    this->device = device;
    this->layout = new VkPipelineLayout();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = shaderLayout->getLayout();

    if (vkCreatePipelineLayout(*device, &pipelineLayoutInfo, nullptr, layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}
VkPipelineLayout* PipelineLayout::getLayout() { return layout; }
PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(*device, *layout, nullptr);
    delete layout;
}