#include "Image.hpp"

Image::Image(Device& device, uint32_t width, uint32_t height, VkFormat format,
             VkImageTiling tiling, VkImageUsageFlags usage,
             VkMemoryPropertyFlags properties, uint32_t layers)
    : device(device) {
    this->extent = {width, height, 1};
    this->layers = layers;
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = this->extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = layers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

    if (vkCreateImage(device.getDevice(), &imageInfo, nullptr, &image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device.getDevice(), image, &memRequirements);

    memory = std::make_unique<Memory>(device, memRequirements, properties);

    vkBindImageMemory(device.getDevice(), image, memory->getMemory(), 0);
}

Image::Image(Image&& img)
    : device(img.device),
      extent(img.extent),
      image(NULL),
      layers(img.layers),
      memory() {
    *this = std::move(img);
}

Image& Image::operator=(Image&& img) {
    assert(img.device == this->device);
    std::swap(this->image, img.image);
    std::swap(this->memory, img.memory);
    return *this;
}

std::vector<std::shared_ptr<Image>> createImagesForSwapchain(
    Device& device, VkSwapchainKHR swapchain, VkExtent2D extent) {
    uint32_t count;
    std::vector<std::shared_ptr<Image>> swapChainImages;
    vkGetSwapchainImagesKHR(device.getDevice(), swapchain, &count, NULL);
    std::vector<VkImage> images(count);
    vkGetSwapchainImagesKHR(device.getDevice(), swapchain, &count,
                            images.data());
    for (int i = 0; i < count; i++)
        swapChainImages.emplace_back(
            std::make_shared<Image>(device, images[i], extent));
    return swapChainImages;
}

Image::Image(Device& device, VkImage image, VkExtent2D extent)
    : device(device) {
    this->image = image;
    this->memory = NULL;
    this->extent = {extent.width, extent.height, 1};
}

VkImage Image::getImage() { return image; }

Device& Image::getDevice() { return device; }

std::unique_ptr<Memory>& Image::getMemory() { return memory; }

VkExtent3D Image::getExtent() const { return extent; }

uint32_t Image::getLayers() const { return layers; }

VkSubresourceLayout Image::getImageSubresourceLayout(uint32_t mipLevel,
                                                     uint32_t arrayLayer,
                                                     VkImageAspectFlags flags) {
    VkSubresourceLayout result;
    VkImageSubresource sub{
        .aspectMask = flags, .mipLevel = mipLevel, .arrayLayer = arrayLayer};
    vkGetImageSubresourceLayout(device.getDevice(), image, &sub, &result);
    return result;
}

void Image::write(const void* data, VkDeviceSize sz, VkDeviceSize offset) {
    memory->write(data, sz, offset);
}

Image::~Image() {
    if (memory) vkDestroyImage(device.getDevice(), image, NULL);
}