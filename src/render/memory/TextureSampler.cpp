#include "TextureSampler.hpp"

TextureSampler::TextureSampler(VkPhysicalDevice* physicalDevice,
                               Device* device) {
    sampler = new VkSampler();
    this->device = device;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(*physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(*(device->getDevice()), &samplerInfo, nullptr,
                        sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VkDescriptorImageInfo* TextureSampler::createImageInfo(ImageView* view) {
    VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo();
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo->imageView = *(view->getView());
    imageInfo->sampler = *sampler;
    return imageInfo;
}

VkSampler* TextureSampler::getSampler() { return sampler; }

TextureSampler::~TextureSampler() {
    vkDestroySampler(*(device->getDevice()), *sampler, nullptr);
    delete sampler;
}