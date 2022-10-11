#include "Image.hpp"

Image::Image(VkPhysicalDevice* physicalDevice, Device* device, uint32_t width,
             uint32_t height, VkFormat format, VkImageTiling tiling,
             VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    this->device = device;
    this->image = new VkImage();
    this->extent = {width, height, 1};
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = this->extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

    if (vkCreateImage(*(device->getDevice()), &imageInfo, nullptr, image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(*(device->getDevice()), *image,
                                 &memRequirements);

    memory = new Memory(physicalDevice, device, memRequirements, properties);

    vkBindImageMemory(*(device->getDevice()), *image, *(memory->getMemory()),
                      0);
}

std::vector<Image*> createImagesForSwapchain(Device* device,
                                             VkSwapchainKHR swapchain,
                                             uint32_t* imageCount,
                                             VkExtent2D extent) {
    std::vector<Image*> swapChainImages;
    vkGetSwapchainImagesKHR(*(device->getDevice()), swapchain, imageCount,
                            NULL);
    VkImage* images = new VkImage[*imageCount];
    vkGetSwapchainImagesKHR(*(device->getDevice()), swapchain, imageCount,
                            images);
    for (int i = 0; i < *imageCount; i++)
        swapChainImages.push_back(new Image(device, images + i, extent));
    return swapChainImages;
}

Image::Image(Device* device, VkImage* image, VkExtent2D extent) {
    this->device = device;
    this->image = image;
    this->memory = NULL;
    this->extent = {extent.width, extent.height, 1};
}

VkImage* Image::getImage() { return image; }

Memory* Image::getMemory() { return memory; }

VkExtent3D Image::getExtent() { return extent; }

VkSubresourceLayout Image::getImageSubresourceLayout(uint32_t mipLevel,
                                                     uint32_t arrayLayer,
                                                     VkImageAspectFlags flags) {
    VkSubresourceLayout result;
    VkImageSubresource sub{
        .aspectMask = flags, .mipLevel = mipLevel, .arrayLayer = arrayLayer};
    vkGetImageSubresourceLayout(*(device->getDevice()), *image, &sub, &result);
    return result;
}

void Image::write(const void* data, VkDeviceSize sz, VkDeviceSize offset) {
    memory->write(data, sz, offset);
}

Image::~Image() {
    if (memory != NULL) {
        vkDestroyImage(*(device->getDevice()), *image, NULL);
        delete image;
        delete memory;  // Again I wish I could do that
    }
}