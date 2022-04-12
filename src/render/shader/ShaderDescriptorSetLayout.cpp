#include "ShaderDescriptorSetLayout.hpp"

ShaderDescriptorSetLayout::ShaderDescriptorSetLayout(
    Device* device, VkDescriptorSetLayoutBinding* bindings,
    uint32_t nBindings) {
    this->layout = new VkDescriptorSetLayout();

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = nBindings;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(*(device->getDevice()), &layoutInfo,
                                    nullptr, layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    this->device = device;
}

VkDescriptorSetLayout* ShaderDescriptorSetLayout::getLayout() { return layout; }

ShaderDescriptorSetLayout::~ShaderDescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(*(device->getDevice()), *layout, nullptr);
    delete layout;
}