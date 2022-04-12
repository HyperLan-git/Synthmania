#include "Swapchain.hpp"

Swapchain::Swapchain(Device *device, VkPhysicalDevice *physicalDevice,
                     Window *window, Shader *vertShader, Shader *fragShader,
                     ShaderDescriptorSetLayout *shaderDescriptors,
                     VkSurfaceKHR *surface) {
    SwapchainSupportDetails swapchainSupport =
        querySwapchainSupport(*physicalDevice, *surface);

    this->device = device;
    this->extent = chooseSwapExtent(swapchainSupport.capabilities, window);
    this->physicalDevice = physicalDevice;
    this->swapchain = new VkSwapchainKHR();

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapchainSupport.presentModes);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = *surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {device->getQueue(0)->getFamily(),
                                     device->getQueue(1)->getFamily()};

    if (device->getQueue(1)->getFamily() != device->getQueue(0)->getFamily()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(*(device->getDevice()), &createInfo, nullptr,
                             swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    imageFormat = surfaceFormat.format;

    images = createImagesForSwapchain(device, *swapchain, &imageCount, extent);

    for (uint32_t i = 0; i < images.size(); i++) {
        imageViews.push_back(new ImageView(device, images[i], imageFormat,
                                           VK_IMAGE_ASPECT_COLOR_BIT));
    }

    VkFormat depthFormat = findDepthFormat(*physicalDevice);

    depthImage = new Image(physicalDevice, device, extent.width, extent.height,
                           depthFormat, VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    depthImageView = new ImageView(device, depthImage, depthFormat,
                                   VK_IMAGE_ASPECT_DEPTH_BIT);

    renderPass = new RenderPass(physicalDevice, device, imageFormat);

    for (size_t i = 0; i < imageViews.size(); i++) {
        std::vector<ImageView *> attachments = {imageViews[i], depthImageView};
        framebuffers.push_back(
            new Framebuffer(device, renderPass, extent, attachments));
    }

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShader->toPipeline(),
                                                      fragShader->toPipeline()};
    VkPushConstantRange range;
    range.offset = 0;
    range.size = sizeof(float);
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    pipelineLayout = new PipelineLayout(device, shaderDescriptors, &range);

    graphicsPipeline = new Pipeline(device, pipelineLayout, renderPass,
                                    shaderStages, 2, extent);
}

RenderPass *Swapchain::getRenderPass() { return renderPass; }

std::vector<Framebuffer *> Swapchain::getFramebuffers() { return framebuffers; }

VkExtent2D Swapchain::getExtent() { return extent; }

Pipeline *Swapchain::getPipeline() { return graphicsPipeline; }

VkSwapchainKHR *Swapchain::getSwapchain() { return swapchain; }

Swapchain::~Swapchain() {
    delete depthImageView;
    delete depthImage;

    for (auto framebuffer : framebuffers) {
        delete framebuffer;
    }

    delete graphicsPipeline;
    delete pipelineLayout;
    delete renderPass;

    for (auto imageView : imageViews) {
        delete imageView;
    }

    vkDestroySwapchainKHR(*(device->getDevice()), *swapchain, nullptr);
    delete swapchain;
}