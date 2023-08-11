#include "ShaderDescriptorSetLayout.hpp"

ShaderDescriptorSetLayout::ShaderDescriptorSetLayout(
    Device& device,
    std::initializer_list<VkDescriptorSetLayoutBinding> bindings)
    : device(device) {
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.begin();

    if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr,
                                    &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

ShaderDescriptorSetLayout::ShaderDescriptorSetLayout(
    Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    : device(device) {
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr,
                                    &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

VkDescriptorSetLayout ShaderDescriptorSetLayout::getLayout() { return layout; }

Device& ShaderDescriptorSetLayout::getDevice() { return device; }

ShaderDescriptorSetLayout::~ShaderDescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device.getDevice(), layout, nullptr);
}