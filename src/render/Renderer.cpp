#define STB_IMAGE_IMPLEMENTATION
#include "Renderer.hpp"

Renderer::Renderer(Window* window) {
    this->window = window;
    initVulkan();
}

void Renderer::initVulkan() {
    /*new Model({{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
               {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
               {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
               {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

               {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
               {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
               {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
               {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}},
              {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4});*/
    createInstance();
    surface = instance->createSurface(window);
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    commandPool = new CommandPool(physicalDevice, device);
    Model* model = new Model("resources/room.obj", &physicalDevice, device);
    Image* img = createTextureImage("resources/viking_room.png");
    models.push_back(model);
    textures.push_back(img);
    Entity* e = new Entity(model, img, "Bob");
    entities.push_back(e);
    textureImage =
        createSamplerImage(img->getExtent().width, img->getExtent().height);
    textureImageView =
        new ImageView(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT);
    textureSampler = new TextureSampler(&physicalDevice, device);
    createVertexBuffer(model->toVertexBuffer()->getSize());
    createIndexBuffer(model->toIndicesBuffer()->getSize());
    createUniformBuffers();
    VkDescriptorType types[] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    pool = new ShaderDescriptorPool(device, types, MAX_FRAMES_IN_FLIGHT);
    createDescriptorSets();

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        commandBuffers.push_back(new CommandBuffer(device, commandPool, false));

        imageAvailableSemaphores.push_back(new Semaphore(device));
        renderFinishedSemaphores.push_back(new Semaphore(device));
        inFlightFences.push_back(new Fence(device));
    }
}

void Renderer::render() { drawFrame(); }

Renderer::~Renderer() {
    vkDeviceWaitIdle(*(device->getDevice()));

    delete swapchain;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        delete constantUniformBuffers[i];
        delete uniformBuffers[i];
    }

    delete pool;

    delete textureSampler;
    delete textureImageView;

    delete textureImage;

    delete shaderLayout;

    delete indexBuffer;

    delete vertexBuffer;

    for (Entity* e : entities) delete e;
    for (Model* m : models) delete m;
    for (Image* i : textures) delete i;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        delete renderFinishedSemaphores[i];
        delete imageAvailableSemaphores[i];
        delete inFlightFences[i];
    }

    delete commandPool;

    delete device;

    instance->destroySurface(surface);
    delete instance;
}

void Renderer::recreateSwapChain() {
    uint32_t width = 0, height = 0;
    window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0) {
        window->getFramebufferSize(&width, &height);
        glfwWaitEvents();
    }

    device->wait();

    delete swapchain;
    delete shaderLayout;

    createSwapchain();
}

void Renderer::createInstance() {
    uint32_t ver = VK_MAKE_VERSION(1, 0, 0);
    const char *appName = "Hello Triangle", *engineName = "No Engine";
    if (enableValidationLayers)
        instance =
            new Instance(appName, ver, engineName, ver, VK_API_VERSION_1_0,
                         getRequiredExtensions(), validationLayers);
    else
        instance = new Instance(appName, ver, engineName, ver,
                                VK_API_VERSION_1_0, getRequiredExtensions());
}

void Renderer::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*(instance->getInstance()), &deviceCount,
                               nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*(instance->getInstance()), &deviceCount,
                               devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void Renderer::createSwapchain() {
    VkDescriptorSetLayoutBinding ubo, textureSampler;
    ubo.binding = 0;
    ubo.descriptorCount = 1;
    ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    textureSampler.binding = 1;
    textureSampler.descriptorCount = 1;
    textureSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutBinding bindings[] = {ubo, textureSampler};
    shaderLayout = new ShaderDescriptorSetLayout(device, bindings, 2);

    auto vertShaderCode = readFile("bin/vert.spv");
    auto fragShaderCode = readFile("bin/frag.spv");

    Shader* vertShader = new Shader("main", device, vertShaderCode, true);
    Shader* fragShader = new Shader("main", device, fragShaderCode, false);
    VkPushConstantRange* range = new VkPushConstantRange();
    range->offset = 0;
    range->size = sizeof(EntityData);
    range->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    swapchain = new Swapchain(device, &physicalDevice, window, vertShader,
                              fragShader, shaderLayout, surface, range, 1);
    delete range;
    delete vertShader;
    delete fragShader;
}

void Renderer::createLogicalDevice() {
    std::vector<FamilyPredicate> familyPredicates = {};
    familyPredicates.push_back(
        [](VkPhysicalDevice dev, VkQueueFamilyProperties prop, uint32_t id) {
            return prop.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        });
    familyPredicates.push_back([this](VkPhysicalDevice dev,
                                      VkQueueFamilyProperties prop,
                                      uint32_t id) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(dev, id, *surface,
                                             &presentSupport);
        return presentSupport;
    });
    if (enableValidationLayers) {
        device = new Device(&physicalDevice, deviceExtensions, familyPredicates,
                            validationLayers);
    } else {
        device =
            new Device(&physicalDevice, deviceExtensions, familyPredicates);
    }
}

bool Renderer::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
}

Image* Renderer::createSamplerImage(int width, int height) {
    VkDeviceSize imageSize = width * height * 4;

    Image* image =
        new Image(&physicalDevice, device, width, height,
                  VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return image;
}

Image* Renderer::createTextureImage(const char* path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels =
        stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    Buffer* stagingBuffer = new Buffer(
        &physicalDevice, device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(*(device->getDevice()),
                *(stagingBuffer->getMemory()->getMemory()), 0, imageSize, 0,
                &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(*(device->getDevice()),
                  *(stagingBuffer->getMemory()->getMemory()));

    stbi_image_free(pixels);

    Image* image = new Image(
        &physicalDevice, device, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth),
                      static_cast<uint32_t>(texHeight));
    transitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    delete stagingBuffer;
    return image;
}

void Renderer::transitionImageLayout(Image* image, VkImageLayout oldLayout,
                                     VkImageLayout newLayout) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    commandBuffer->begin();

    commandBuffer->setImageLayout(image, oldLayout, newLayout);

    commandBuffer->end();
    commandBuffer->submit(device->getQueue(0));
    delete commandBuffer;
}

void Renderer::copyBufferToImage(Buffer* buffer, Image* image, uint32_t width,
                                 uint32_t height) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    commandBuffer->begin();

    commandBuffer->copyBufferToImage(buffer, image, {width, height, 1});

    commandBuffer->end();
    commandBuffer->submit(device->getQueue(0));
    delete commandBuffer;
}

void Renderer::copyImage(Image* src, VkImageLayout srcLayout, Image* dst,
                         VkImageLayout dstLayout) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    commandBuffer->begin();

    commandBuffer->copyImage(src, srcLayout, dst, dstLayout);

    commandBuffer->end();
    commandBuffer->submit(device->getQueue(0));
    delete commandBuffer;
}

void Renderer::createVertexBuffer(VkDeviceSize size) {
    vertexBuffer = new Buffer(
        &physicalDevice, device, size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void Renderer::createIndexBuffer(VkDeviceSize size) {
    indexBuffer = new Buffer(
        &physicalDevice, device, size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void Renderer::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject),
                 constantsSize = sizeof(float);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers.push_back(
            new Buffer(&physicalDevice, device, bufferSize,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        constantUniformBuffers.push_back(
            new Buffer(&physicalDevice, device, constantsSize,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    }
}

void Renderer::createDescriptorSets() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        descriptorSets.push_back(
            new ShaderDescriptorSet(device, pool, shaderLayout));
        VkDescriptorBufferInfo* bufferInfo =
            createBufferInfo(uniformBuffers[i]);

        VkDescriptorImageInfo* imageInfo =
            createImageInfo(textureImageView, textureSampler);

        descriptorSets[i]->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                        0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                        bufferInfo, nullptr);

        descriptorSets[i]->updateAccess(
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 1,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, imageInfo);

        delete imageInfo;
        delete bufferInfo;
    }
}

float time_from_start = 0;

void Renderer::recordCommandBuffer(CommandBuffer* commandBuffer,
                                   uint32_t imageIndex) {
    commandBuffer->reset();
    commandBuffer->begin();

    std::vector<VkClearValue> clearValues = {VkClearValue(), VkClearValue()};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    commandBuffer->beginRenderPass(swapchain, imageIndex, clearValues.data(),
                                   2);

    commandBuffer->bindPipeline(swapchain->getPipeline());

    commandBuffer->bindDescriptorSet(swapchain->getPipeline(),
                                     descriptorSets[currentFrame]);

    Model* lastModel = nullptr;
    Image* lastTexture = nullptr;
    for (Entity* e : entities) {
        e->update(time_from_start);
        Model* model = e->getModel();
        Image* texture = e->getTexture();
        if (lastModel != model) {
            model->toVertexBuffer()->copyTo(vertexBuffer, device->getQueue(0),
                                            commandPool);
            model->toIndicesBuffer()->copyTo(indexBuffer, device->getQueue(0),
                                             commandPool);
            commandBuffer->bindVertexBuffers(vertexBuffer, 1);
            commandBuffer->bindIndexBuffer(indexBuffer);
        }
        if (lastTexture != texture) {
            transitionImageLayout(textureImage,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            copyImage(texture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                      textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            transitionImageLayout(textureImage,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        drawEntity(e, commandBuffer);
        lastModel = model;
        lastTexture = texture;
    }

    commandBuffer->endRenderPass();

    commandBuffer->end();
}

#include <stdlib.h>

void Renderer::drawEntity(Entity* entity, CommandBuffer* commandBuffer) {
    Model* model = entity->getModel();

    ShaderData* data = entity->getShaderData();
    commandBuffer->pushConstants(swapchain->getPipeline(),
                                 VK_SHADER_STAGE_VERTEX_BIT, 0, data->data,
                                 data->size);

    commandBuffer->draw(model->getIndexes().size());
    free(data->data);
    delete data;
}

void Renderer::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    time_from_start =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime -
                                                                   startTime)
            .count();
    float x = cos(time_from_start * 2 / 3) / 2,
          y = sin(time_from_start * 5 / 3) / 2;  // Lissajous :)

    x = y = 0;
    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(glm::vec3(x, y, 1.0f), glm::vec3(x, y, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f));
    ubo.proj = glm::perspective(
        glm::radians(45.0f),  // MASTER FOV
        swapchain->getExtent().width / (float)swapchain->getExtent().height,
        0.1f, 10.0f);
    ubo.proj[1][1] *= -1;  // Invert y axis cause I like my y axis positive up

    void* data = NULL;
    vkMapMemory(*(device->getDevice()),
                *(uniformBuffers[currentImage]->getMemory()->getMemory()), 0,
                sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(*(device->getDevice()),
                  *(uniformBuffers[currentImage]->getMemory()->getMemory()));
}

void Renderer::drawFrame() {
    inFlightFences[currentFrame]->wait();

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        *(device->getDevice()), *(swapchain->getSwapchain()), UINT64_MAX,
        *(imageAvailableSemaphores[currentFrame]->getSemaphore()),
        VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(currentFrame);

    inFlightFences[currentFrame]->reset();

    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    commandBuffers[currentFrame]->submit(
        device->getQueue(0), imageAvailableSemaphores[currentFrame],
        renderFinishedSemaphores[currentFrame], inFlightFences[currentFrame]);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores =
        renderFinishedSemaphores[currentFrame]->getSemaphore();

    VkSwapchainKHR* swapchains = swapchain->getSwapchain();
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;

    presentInfo.pImageIndices = &imageIndex;

    result =
        vkQueuePresentKHR(*(device->getQueue(1)->getQueue()), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        window->hasResized()) {
        window->setResized(false);
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

bool Renderer::isDeviceSuitable(VkPhysicalDevice device) {
    std::vector<FamilyPredicate> familyPredicates = {
        [](VkPhysicalDevice dev, VkQueueFamilyProperties prop, uint32_t id) {
            return prop.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        },
        [this](VkPhysicalDevice dev, VkQueueFamilyProperties prop,
               uint32_t id) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, id, *surface,
                                                 &presentSupport);
            return presentSupport;
        }};
    std::vector<uint32_t> families =
        findQueueFamilies(device, familyPredicates);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapchainSupportDetails swapChainSupport =
            querySwapchainSupport(device, *surface);
        swapChainAdequate = !swapChainSupport.formats.empty() &&
                            !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return families.size() == familyPredicates.size() && extensionsSupported &&
           swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool Renderer::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

std::vector<const char*> Renderer::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}