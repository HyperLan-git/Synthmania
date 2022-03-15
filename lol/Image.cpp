#include "Image.hpp"

Image::Image(VkPhysicalDevice* physicalDevice, VkDevice* device, uint32_t width,
             uint32_t height, VkFormat format, VkImageTiling tiling,
             VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    this->device = device;
    this->image = new VkImage();
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(*device, &imageInfo, nullptr, image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(*device, *image, &memRequirements);

    memory = new Memory(physicalDevice, device, memRequirements, properties);

    vkBindImageMemory(*device, *image, *(memory->getMemory()), 0);
}

std::vector<Image*> createImagesForSwapchain(VkDevice* device,
                                             VkSwapchainKHR swapChain,
                                             uint32_t* imageCount) {
    std::vector<Image*> swapChainImages;
    vkGetSwapchainImagesKHR(*device, swapChain, imageCount, nullptr);
    VkImage* images = new VkImage[*imageCount];
    vkGetSwapchainImagesKHR(*device, swapChain, imageCount, images);
    for (int i = 0; i < *imageCount; i++)
        swapChainImages.push_back(new Image(device, images + i));
    return swapChainImages;
}

Image::Image(VkDevice* device, VkImage* image) {
    this->device = device;
    this->image = image;
    this->memory = nullptr;  // TODO find a way to get associated memory
}

VkImage* Image::getImage() { return image; }

Memory* Image::getMemory() { return memory; }

Image::~Image() {
    vkDestroyImage(*device, *image, nullptr);
    delete image;
    if (memory != nullptr) delete memory;  // Again I wish I could do that
}