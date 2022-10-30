#include "Swapchain.hpp"

Swapchain::Swapchain(Device *device, VkPhysicalDevice *physicalDevice,
                     Window *window, VkSurfaceKHR *surface) {
    this->device = device;
    this->physicalDevice = physicalDevice;
    this->swapchain = new VkSwapchainKHR();

    SwapchainSupportDetails swapchainSupport =
        querySwapchainSupport(*physicalDevice, *surface);
    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapchainSupport.presentModes);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }

    uint32_t queueFamilyIndices[] = {
        device->getQueue("main")->getFamily(),
        device->getQueue("secondary")->getFamily()};

    VkSwapchainCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = *surface;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    // TODO oldSwapchain
    createInfo.oldSwapchain = NULL;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 1;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    // Keep the choosing of the extent close to createSwapchain
    swapchainSupport = querySwapchainSupport(*physicalDevice, *surface);
    this->extent = chooseSwapExtent(swapchainSupport.capabilities, window);
    createInfo.imageExtent = extent;

    if (vkCreateSwapchainKHR(*(device->getDevice()), &createInfo, nullptr,
                             swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    imageFormat = surfaceFormat.format;

    images = createImagesForSwapchain(device, *swapchain, &imageCount, extent);

    for (uint32_t i = 0; i < images.size(); i++)
        imageViews.push_back(new ImageView(device, images[i], imageFormat,
                                           VK_IMAGE_ASPECT_COLOR_BIT,
                                           "swapimage" + i));

    VkFormat depthFormat = findDepthFormat(*physicalDevice);

    depthImage = new Image(physicalDevice, device, extent.width, extent.height,
                           depthFormat, VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    depthImageView = new ImageView(device, depthImage, depthFormat,
                                   VK_IMAGE_ASPECT_DEPTH_BIT, "depthimage");

    renderPass = new RenderPass(physicalDevice, device, imageFormat);

    for (size_t i = 0; i < imageViews.size(); i++) {
        std::vector<ImageView *> attachments = {imageViews[i], depthImageView};
        framebuffers.push_back(
            new Framebuffer(device, renderPass, extent, attachments));
    }
}

RenderPass *Swapchain::getRenderPass() { return renderPass; }

std::vector<Framebuffer *> Swapchain::getFramebuffers() { return framebuffers; }

VkExtent2D Swapchain::getExtent() { return extent; }

VkSwapchainKHR *Swapchain::getSwapchain() { return swapchain; }

Swapchain::~Swapchain() {
    delete depthImageView;
    delete depthImage;

    for (auto framebuffer : framebuffers) {
        delete framebuffer;
    }

    delete renderPass;

    delete[] images[0]->getImage();
    for (auto img : images) delete img;
    for (auto imageView : imageViews) delete imageView;

    vkDestroySwapchainKHR(*(device->getDevice()), *swapchain, nullptr);
    delete swapchain;
}