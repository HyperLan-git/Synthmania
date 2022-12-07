// Must appear only once in a source file
#define STB_IMAGE_IMPLEMENTATION
#include "Renderer.hpp"

#include <stdlib.h>

#include <map>

Renderer::Renderer(Game* game, Window* window) {
    this->game = game;
    this->window = window;
    initVulkan();
}

void Renderer::initVulkan() {
    if (!glfwVulkanSupported()) throw std::runtime_error("Cannot use vulkan !");
    createInstance();
    surface = instance->createSurface(window);
    pickPhysicalDevice();
    createLogicalDevice();
    // TODO make a function called setAllNames
    DebugFunc functions = getDebugFunctions(instance);
    // Error for some reason
    /*setName(functions, device, "GPU", VK_OBJECT_TYPE_PHYSICAL_DEVICE,
            physicalDevice);*/
    setName(functions, device, "Instance", VK_OBJECT_TYPE_INSTANCE,
            *(instance->getInstance()));
    setName(functions, device, "mainSurface", VK_OBJECT_TYPE_SURFACE_KHR,
            *surface);
    setName(functions, device, "secondaryQueue", VK_OBJECT_TYPE_QUEUE,
            *(device->getQueue("secondary")->getQueue()));
    setName(functions, device, "mainQueue", VK_OBJECT_TYPE_QUEUE,
            *(device->getQueue("main")->getQueue()));
    commandPool = new CommandPool(device);
    setName(functions, device, "Command Pool", VK_OBJECT_TYPE_COMMAND_POOL,
            *(commandPool->getPool()));
    textHandler = new TextHandler(&physicalDevice, device, FONT_SIZE);
    createSwapchain();
    int i = 0;
    for (Framebuffer* f : swapchain->getFramebuffers()) {
        std::string name = "Framebuffer_";
        name.append(std::to_string(i++));
        setName(functions, device, name, VK_OBJECT_TYPE_FRAMEBUFFER,
                *(f->getFramebuffer()));
    }
    setName(functions, device, "RenderPass", VK_OBJECT_TYPE_RENDER_PASS,
            *(swapchain->getRenderPass()->getPass()));

    createGraphicsPipeline();

    createGuiPipeline();

    createMainPipeline();

    guiModel = new Model({{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
                          {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
                          {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
                          {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}}},
                         {0, 2, 3, 0, 1, 2}, &physicalDevice, device);
    setName(functions, device, "Gui index buffer", VK_OBJECT_TYPE_BUFFER,
            *(guiModel->toIndicesBuffer()->getBuffer()));
    setName(functions, device, "Gui vertex buffer", VK_OBJECT_TYPE_BUFFER,
            *(guiModel->toVertexBuffer()->getBuffer()));

    textureSampler = new TextureSampler(&physicalDevice, device);
    setName(functions, device, "3D sampler", VK_OBJECT_TYPE_SAMPLER,
            *(textureSampler->getSampler()));
    Model* model =
        new Model("resources/models/room.obj", &physicalDevice, device);
    models.push_back(model);
    // TODO get max buffer usage
    createVertexBuffer(model->toVertexBuffer()->getSize());
    setName(functions, device, "Main vertex buffer", VK_OBJECT_TYPE_BUFFER,
            *(vertexBuffer->getBuffer()));
    createIndexBuffer(model->toIndicesBuffer()->getSize());
    setName(functions, device, "Main index buffer", VK_OBJECT_TYPE_BUFFER,
            *(indexBuffer->getBuffer()));

    createUniformBuffers();
    createCommandBuffers();
}

void Renderer::createCommandBuffers() {
    DebugFunc functions = getDebugFunctions(instance);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        commandBuffers.push_back(new CommandBuffer(device, commandPool, false));
        setName(functions, device, "Main command buffer " + std::to_string(i),
                VK_OBJECT_TYPE_COMMAND_BUFFER,
                *(commandBuffers[i]->getBuffer()));

        imageAvailableSemaphores.push_back(new Semaphore(device));
        setName(functions, device,
                "Image available semaphore " + std::to_string(i),
                VK_OBJECT_TYPE_SEMAPHORE,
                *(imageAvailableSemaphores[i]->getSemaphore()));
        renderFinishedSemaphores.push_back(new Semaphore(device));
        setName(functions, device, "Main semaphore " + std::to_string(i),
                VK_OBJECT_TYPE_SEMAPHORE,
                *(renderFinishedSemaphores[i]->getSemaphore()));
        inFlightFences.push_back(new Fence(device));
        setName(functions, device, "Main semaphore " + std::to_string(i),
                VK_OBJECT_TYPE_FENCE, *(inFlightFences[i]->getFence()));
    }
    renderCommandBuffer = new CommandBuffer(device, commandPool, false);
    setName(functions, device, "Render command buffer",
            VK_OBJECT_TYPE_COMMAND_BUFFER, *(renderCommandBuffer->getBuffer()));
}

void Renderer::createSwapchain() {
    uint32_t w = 0, h = 0;
    window->getFramebufferSize(&w, &h);
    while (w == 0 || h == 0) {
        glfwWaitEvents();
        window->getFramebufferSize(&w, &h);
    }
    DebugFunc functions = getDebugFunctions(instance);
    swapchain = new Swapchain(device, &physicalDevice, window, surface);
    setName(functions, device, "Swapchain", VK_OBJECT_TYPE_SWAPCHAIN_KHR,
            *(swapchain->getSwapchain()));
    // w *= 1.5;
    // h *= 1.5;
    renderImage = new Image(
        &physicalDevice, device, w, h, VK_FORMAT_B8G8R8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    renderImageView =
        new ImageView(device, renderImage, VK_FORMAT_B8G8R8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT, "render");
    setName(functions, device, "render image", VK_OBJECT_TYPE_IMAGE,
            *(renderImage->getImage()));
    setName(functions, device, "render image view", VK_OBJECT_TYPE_IMAGE_VIEW,
            *(renderImageView->getView()));
    sampler = new TextureSampler(&physicalDevice, device);
    setName(functions, device, "render image sampler", VK_OBJECT_TYPE_SAMPLER,
            *(sampler->getSampler()));
    VkFormat depthFormat = findDepthFormat(physicalDevice);

    depthImage = new Image(&physicalDevice, device, w, h, depthFormat,
                           VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    depthImageView = new ImageView(device, depthImage, depthFormat,
                                   VK_IMAGE_ASPECT_DEPTH_BIT, "depth image");
    setName(functions, device, "depth image", VK_OBJECT_TYPE_IMAGE,
            *(depthImage->getImage()));
    setName(functions, device, "depth image view", VK_OBJECT_TYPE_IMAGE_VIEW,
            *(depthImageView->getView()));

    renderPass =
        new RenderPass(&physicalDevice, device, VK_FORMAT_B8G8R8A8_SRGB,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    setName(functions, device, "render pass", VK_OBJECT_TYPE_RENDER_PASS,
            *(renderPass->getPass()));

    framebuffer = new Framebuffer(device, swapchain->getRenderPass(), {w, h},
                                  {renderImageView, depthImageView});
    setName(functions, device, "framebuffer", VK_OBJECT_TYPE_FRAMEBUFFER,
            *(framebuffer->getFramebuffer()));
}

void Renderer::loadTextures(std::map<std::string, std::string> textures) {
    for (auto entry : textures) {
        this->textures.push_back(
            readTexture(entry.second.c_str(), entry.first.c_str()));
    }
    for (ImageView* view : textHandler->getTextures())
        this->textures.push_back(view);

    uint32_t type_sz = 2 * this->textures.size();
    std::vector<VkDescriptorType> types(type_sz);
    for (size_t i = 0; i < textures.size(); i++) {
        types[2 * i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        types[2 * i + 1] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
    VkDescriptorType* tp = types.data();

    pool = new ShaderDescriptorPool(device, tp, type_sz);
    setName(getDebugFunctions(instance), device, "mainDescriptorPool",
            VK_OBJECT_TYPE_DESCRIPTOR_POOL, *(pool->getPool()));
    uint32_t sz = textures.size() * MAX_FRAMES_IN_FLIGHT;
    if (sz == 0) sz = 1;
    uint32_t i[] = {sz, sz};
    VkDescriptorType type[] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    guiModule->recreateDescriptorPool(type, i, 2);
    renderDescriptorPool = new ShaderDescriptorPool(device, type, 2);
    setName(getDebugFunctions(instance), device, "renderDescriptorPool",
            VK_OBJECT_TYPE_DESCRIPTOR_POOL, *(renderDescriptorPool->getPool()));
    // This is needed trust me
    recreateSwapchain();
}

VkPhysicalDevice* Renderer::getPhysicalDevice() { return &physicalDevice; }
Device* Renderer::getDevice() { return device; }

Instance* Renderer::getInstance() { return instance; }

TextHandler* Renderer::getTextHandler() { return textHandler; }

void Renderer::setStartTime(double start) { this->startTime = start; }

void Renderer::addModel(Model* model) { models.push_back(model); }

std::vector<ImageView*> Renderer::getTextures() { return textures; }

void Renderer::render() { drawFrame(); }

Renderer::~Renderer() {
    device->wait();

    delete swapchain;

    delete renderImageView;
    delete renderImage;
    delete depthImage;
    delete depthImageView;
    delete renderPass;
    delete framebuffer;
    delete renderCommandBuffer;
    delete renderDescriptor;
    delete uniformBuffer;
    delete renderPipeline;
    delete renderPipelineLayout;
    delete sampler;
    delete renderLayout;

    delete graphicsPipelineLayout;
    delete graphicsPipeline;

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        delete constantBuffers[i];
        delete uniformBuffers[i];
        delete guiConstantBuffers[i];
        delete guiUniformBuffers[i];
    }

    for (int i = 0; i < descriptorSets.size(); i++) delete descriptorSets[i];

    delete pool;
    delete renderDescriptorPool;

    delete textureSampler;

    delete shaderLayout;

    delete guiModule;

    delete indexBuffer;

    delete vertexBuffer;

    delete guiModel;

    for (Model* m : models) delete m;
    for (ImageView* i : textures) {
        delete i->getImage();
        delete i;
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        delete renderFinishedSemaphores[i];
        delete imageAvailableSemaphores[i];
        delete inFlightFences[i];
    }

    for (CommandBuffer* b : commandBuffers) delete b;
    delete commandPool;

    delete textHandler;

    delete device;

    instance->destroySurface(surface);
    delete instance;
}

void Renderer::recreateSwapchain() {
    device->wait();

    delete swapchain;
    delete renderImageView;
    delete renderImage;
    delete sampler;
    delete depthImage;
    delete depthImageView;
    delete renderPass;
    delete framebuffer;
    delete renderPipeline;
    delete renderLayout;
    delete renderDescriptor;
    delete renderPipelineLayout;
    delete renderDescriptorPool;

    delete graphicsPipeline;
    delete graphicsPipelineLayout;

    delete shaderLayout;
    delete guiModule;
    for (auto d : descriptorSets) delete d;
    descriptorSets.clear();

    createSwapchain();

    createGraphicsPipeline();
    createGuiPipeline();
    createMainPipeline();
    VkDescriptorType type[] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    renderDescriptorPool = new ShaderDescriptorPool(device, type, 2);

    createDescriptorSets();
}

void Renderer::createInstance() {
    uint32_t ver = VK_MAKE_VERSION(1, 0, 0);
    const char *appName = "Synthmania", *engineName = "Rebind";
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
    vkEnumeratePhysicalDevices(*(instance->getInstance()), &deviceCount, NULL);

    if (deviceCount == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*(instance->getInstance()), &deviceCount,
                               devices.data());

    for (const auto& device : devices)
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }

    if (physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("failed to find a suitable GPU!");
}

void Renderer::createGraphicsPipeline() {
    VkDescriptorSetLayoutBinding ubo, textureSampler;
    ubo.binding = 0;
    ubo.descriptorCount = 1;
    ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    textureSampler.binding = 1;
    textureSampler.descriptorCount = 1;
    textureSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureSampler.pImmutableSamplers = NULL;
    VkDescriptorSetLayoutBinding bindings[] = {ubo, textureSampler};
    shaderLayout = new ShaderDescriptorSetLayout(device, bindings, 2);

    auto vertShaderCode = readFile("bin/def.vert.spv");
    auto geomShaderCode = readFile("bin/def.geom.spv");
    auto fragShaderCode = readFile("bin/def.frag.spv");

    Shader vertShader =
        Shader("main", device, vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT);
    Shader geomShader =
        Shader("main", device, geomShaderCode, VK_SHADER_STAGE_GEOMETRY_BIT);
    Shader fragShader =
        Shader("main", device, fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT);
    DebugFunc functions = getDebugFunctions(instance);
    setName(functions, device, "3D vertex shader", VK_OBJECT_TYPE_SHADER_MODULE,
            *(vertShader.getModule()));
    setName(functions, device, "3D geometry shader",
            VK_OBJECT_TYPE_SHADER_MODULE, *(geomShader.getModule()));
    setName(functions, device, "3D fragment shader",
            VK_OBJECT_TYPE_SHADER_MODULE, *(fragShader.getModule()));
    VkPushConstantRange range;
    range.size = sizeof(EntityData);
    range.offset = 0;
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShader.toPipeline(),
                                                      geomShader.toPipeline(),
                                                      fragShader.toPipeline()};

    graphicsPipelineLayout =
        new PipelineLayout(device, shaderLayout, 1, &range);
    setName(functions, device, "3D pipeline layout",
            VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            *(graphicsPipelineLayout->getLayout()));

    graphicsPipeline =
        new Pipeline(device, graphicsPipelineLayout, swapchain->getRenderPass(),
                     shaderStages, 3, swapchain->getExtent());
    setName(functions, device, "3D pipeline", VK_OBJECT_TYPE_PIPELINE,
            *(graphicsPipeline->getPipeline()));
}

void Renderer::createGuiPipeline() {
    VkDescriptorSetLayoutBinding ubo, textureSampler;
    ubo.binding = 0;
    ubo.descriptorCount = 1;
    ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |
                     VK_SHADER_STAGE_FRAGMENT_BIT;
    textureSampler.binding = 1;
    textureSampler.descriptorCount = 1;
    textureSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureSampler.pImmutableSamplers = NULL;
    VkDescriptorSetLayoutBinding bindings[] = {ubo, textureSampler};

    auto vertShaderCode = readFile(guiVertShader);
    auto geomShaderCode = readFile(guiGeomShader);
    auto fragShaderCode = readFile(guiFragShader);

    Shader vertShader =
        Shader("main", device, vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT);
    Shader geomShader =
        Shader("main", device, geomShaderCode, VK_SHADER_STAGE_GEOMETRY_BIT);
    Shader fragShader =
        Shader("main", device, fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT);

    DebugFunc functions = getDebugFunctions(instance);
    setName(functions, device, "2D vertex shader", VK_OBJECT_TYPE_SHADER_MODULE,
            *(vertShader.getModule()));
    setName(functions, device, "2D geom shader", VK_OBJECT_TYPE_SHADER_MODULE,
            *(geomShader.getModule()));
    setName(functions, device, "2D fragment shader",
            VK_OBJECT_TYPE_SHADER_MODULE, *(fragShader.getModule()));
    VkPushConstantRange range;
    range.offset = 0;
    range.size = sizeof(GuiData);
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShader.toPipeline(),
                                                      geomShader.toPipeline(),
                                                      fragShader.toPipeline()};

    uint32_t sz = textures.size() * MAX_FRAMES_IN_FLIGHT;
    if (sz == 0) sz = 1;
    uint32_t nDescriptorSets[] = {sz, sz};
    guiModule = new RenderModule(
        instance, &physicalDevice, device, "2D", swapchain->getExtent().width,
        swapchain->getExtent().height, renderPass, bindings, nDescriptorSets, 2,
        shaderStages, 3, &range, 1);
}

void Renderer::createMainPipeline() {
    VkDescriptorSetLayoutBinding ubo, textureSampler;
    ubo.binding = 0;
    ubo.descriptorCount = 1;
    ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |
                     VK_SHADER_STAGE_FRAGMENT_BIT;
    textureSampler.binding = 1;
    textureSampler.descriptorCount = 1;
    textureSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureSampler.pImmutableSamplers = NULL;
    VkDescriptorSetLayoutBinding bindings[] = {ubo, textureSampler};
    renderLayout = new ShaderDescriptorSetLayout(device, bindings, 2);
    DebugFunc functions = getDebugFunctions(instance);
    setName(functions, device, "render layout",
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, *(renderLayout->getLayout()));

    auto vertShaderCode = readFile(finalVertShader);
    auto geomShaderCode = readFile(finalGeomShader);
    auto fragShaderCode = readFile(finalFragShader);

    Shader vertShader =
        Shader("main", device, vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT);
    Shader geomShader =
        Shader("main", device, geomShaderCode, VK_SHADER_STAGE_GEOMETRY_BIT);
    Shader fragShader =
        Shader("main", device, fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT);
    setName(functions, device, "Pass vertex shader",
            VK_OBJECT_TYPE_SHADER_MODULE, *(vertShader.getModule()));
    setName(functions, device, "Pass geometry shader",
            VK_OBJECT_TYPE_SHADER_MODULE, *(geomShader.getModule()));
    setName(functions, device, "Pass fragment shader",
            VK_OBJECT_TYPE_SHADER_MODULE, *(fragShader.getModule()));

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShader.toPipeline(),
                                                      geomShader.toPipeline(),
                                                      fragShader.toPipeline()};

    renderPipelineLayout = new PipelineLayout(device, renderLayout, 0, NULL);
    setName(functions, device, "Pass pipeline layout",
            VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            *(renderPipelineLayout->getLayout()));

    uint32_t w, h;
    window->getFramebufferSize(&w, &h);
    while (w == 0 || h == 0) {
        glfwWaitEvents();
        window->getFramebufferSize(&w, &h);
    }
    // w *= 1.5;
    // h *= 1.5;

    renderPipeline =
        new Pipeline(device, renderPipelineLayout, swapchain->getRenderPass(),
                     shaderStages, 3, VkExtent2D({w, h}));
    setName(functions, device, "Pass pipeline", VK_OBJECT_TYPE_PIPELINE,
            *(renderPipeline->getPipeline()));
}

void Renderer::createLogicalDevice() {
    std::map<std::string, FamilyPredicate> familyPredicates = {
        {"main",
         [](VkPhysicalDevice dev, VkQueueFamilyProperties prop, uint32_t id) {
             return prop.queueFlags & VK_QUEUE_GRAPHICS_BIT;
         }},
        {"secondary",
         [this](VkPhysicalDevice dev, VkQueueFamilyProperties prop,
                uint32_t id) {
             VkBool32 presentSupport = false;
             vkGetPhysicalDeviceSurfaceSupportKHR(dev, id, *surface,
                                                  &presentSupport);
             return presentSupport;
         }},
        {"compute",
         [](VkPhysicalDevice dev, VkQueueFamilyProperties prop, uint32_t id) {
             return prop.queueFlags & VK_QUEUE_COMPUTE_BIT;
         }}};
    device = new Device(&physicalDevice, deviceExtensions, familyPredicates,
                        (enableValidationLayers ? validationLayers
                                                : std::vector<const char*>()));
    setName(getDebugFunctions(instance), device, "Device",
            VK_OBJECT_TYPE_DEVICE, *(device->getDevice()));
}

bool Renderer::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Renderer::addTexture(Image* texture, const char* name) {
    ImageView* view = new ImageView(device, texture, VK_FORMAT_R8G8B8A8_SRGB,
                                    VK_IMAGE_ASPECT_COLOR_BIT, name);
    std::string n = name;
    n.append("_view");
    setName(getDebugFunctions(instance), device, n, VK_OBJECT_TYPE_IMAGE_VIEW,
            *(view->getView()));
    this->textures.push_back(view);
}

ImageView* Renderer::readTexture(const char* path, const char* name) {
    Image* tex = createTextureImage(path);
    ImageView* result = new ImageView(device, tex, VK_FORMAT_R8G8B8A8_SRGB,
                                      VK_IMAGE_ASPECT_COLOR_BIT, name);
    std::string n = name;
    n.append("_view");
    setName(getDebugFunctions(instance), device, n, VK_OBJECT_TYPE_IMAGE_VIEW,
            *(result->getView()));

    return result;
}

Image* Renderer::createTextureImage(const char* path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels =
        stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error(std::string(path) +
                                 " failed to load texture image!");
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

    Image* image = new Image(&physicalDevice, device, texWidth, texHeight,
                             VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                 VK_IMAGE_USAGE_SAMPLED_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth),
                      static_cast<uint32_t>(texHeight));
    transitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    delete stagingBuffer;
    return image;
}

void Renderer::transitionImageLayout(Image* image, VkImageLayout oldLayout,
                                     VkImageLayout newLayout) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    setName(getDebugFunctions(this->instance), device, "transitioncmdbuf",
            VK_OBJECT_TYPE_COMMAND_BUFFER, *(commandBuffer->getBuffer()));
    commandBuffer->begin();

    commandBuffer->setImageLayout(image, oldLayout, newLayout);

    commandBuffer->end();
    commandBuffer->submit(device->getQueue("secondary"));
    delete commandBuffer;
}

void Renderer::copyBufferToImage(Buffer* buffer, Image* image, uint32_t width,
                                 uint32_t height) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    setName(getDebugFunctions(this->instance), device, "copybufToImgcmdbuf",
            VK_OBJECT_TYPE_COMMAND_BUFFER, *(commandBuffer->getBuffer()));
    commandBuffer->begin();

    commandBuffer->copyBufferToImage(buffer, image, {width, height, 1});

    commandBuffer->end();
    commandBuffer->submit(device->getQueue("secondary"));
    delete commandBuffer;
}

void Renderer::copyImage(Image* src, VkImageLayout srcLayout, Image* dst,
                         VkImageLayout dstLayout) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    setName(getDebugFunctions(this->instance), device, "copyImgcmdbuf",
            VK_OBJECT_TYPE_COMMAND_BUFFER, *(commandBuffer->getBuffer()));
    commandBuffer->begin();

    commandBuffer->copyImage(src, srcLayout, dst, dstLayout);

    commandBuffer->end();
    commandBuffer->submit(device->getQueue("secondary"));
    delete commandBuffer;
}

void Renderer::convertImage(Image* src, VkImageLayout srcImageLayout,
                            Image* dst, VkImageLayout dstImageLayout,
                            VkFilter filter) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    setName(getDebugFunctions(this->instance), device, "convertImgcmdbuf",
            VK_OBJECT_TYPE_COMMAND_BUFFER, *(commandBuffer->getBuffer()));
    commandBuffer->begin();

    commandBuffer->convertImage(src, srcImageLayout, dst, dstImageLayout,
                                filter);

    commandBuffer->end();
    commandBuffer->submit(device->getQueue("secondary"));
    delete commandBuffer;
}

void Renderer::createVertexBuffer(VkDeviceSize size) {
    vertexBuffer = new Buffer(
        &physicalDevice, device, size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    setName(getDebugFunctions(instance), device, "main vertex buffer",
            VK_OBJECT_TYPE_BUFFER, *(vertexBuffer->getBuffer()));
}

void Renderer::createIndexBuffer(VkDeviceSize size) {
    indexBuffer = new Buffer(
        &physicalDevice, device, size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    setName(getDebugFunctions(instance), device, "main index buffer",
            VK_OBJECT_TYPE_BUFFER, *(indexBuffer->getBuffer()));
}

void Renderer::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers.push_back(
            new Buffer(&physicalDevice, device, bufferSize,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        constantBuffers.push_back(
            new Buffer(&physicalDevice, device, sizeof(EntityData),
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        guiUniformBuffers.push_back(
            new Buffer(&physicalDevice, device, guiUBOSize,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        guiConstantBuffers.push_back(
            new Buffer(&physicalDevice, device, sizeof(GuiData),
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    }
    uniformBuffer = new Buffer(&physicalDevice, device, finalUBOSize,
                               VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void Renderer::updateDescriptorSet(ShaderDescriptorSet* descriptor,
                                   ImageView* texture, TextureSampler* sampler,
                                   Buffer* uniformBuffer) {
    VkDescriptorBufferInfo* bufferInfo = uniformBuffer->createBufferInfo();

    VkDescriptorImageInfo* imageInfo = sampler->createImageInfo(texture);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, bufferInfo,
                             NULL);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 1,
                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, NULL,
                             imageInfo);

    delete imageInfo;
    delete bufferInfo;
}

void Renderer::createDescriptorSets() {
    size_t j = 0;
    DebugFunc functions = getDebugFunctions(instance);
    for (ImageView* img : textures) {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            descriptorSets.push_back(
                new ShaderDescriptorSet(device, pool, shaderLayout));
            std::string name = "descriptor_";
            setName(functions, device, name + "3D_" + std::to_string(i),
                    VK_OBJECT_TYPE_DESCRIPTOR_SET,
                    *(descriptorSets[i]->getSet()));

            updateDescriptorSet(descriptorSets[j + i], img, textureSampler,
                                uniformBuffers[i]);

            guiModule->addDescriptorSet(img, guiUniformBuffers[i]);
        }
        j += MAX_FRAMES_IN_FLIGHT;
    }
    renderDescriptor =
        new ShaderDescriptorSet(device, renderDescriptorPool, renderLayout);
    setName(functions, device, "render descriptor",
            VK_OBJECT_TYPE_DESCRIPTOR_SET, *(renderDescriptor->getSet()));
    updateDescriptorSet(renderDescriptor, renderImageView, sampler,
                        uniformBuffer);
}

void Renderer::loadGuiShaders(std::string vShader, std::string gShader,
                              std::string fShader, VkDeviceSize guiUBOSize) {
    this->guiUBOSize = guiUBOSize;
    if (!vShader.empty()) this->guiVertShader = vShader;
    if (!gShader.empty()) this->guiGeomShader = gShader;
    if (!fShader.empty()) this->guiFragShader = fShader;

    device->wait();
    for (int i = 0; i < descriptorSets.size(); i++) delete descriptorSets[i];
    descriptorSets.clear();
    delete renderDescriptor;
    delete pool;
    delete renderDescriptorPool;
    // TODO use vkResetDescriptorPool and other pools aswell
    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        delete constantBuffers[i];
        delete uniformBuffers[i];
        delete guiConstantBuffers[i];
        delete guiUniformBuffers[i];
    }
    constantBuffers.clear();
    uniformBuffers.clear();
    guiConstantBuffers.clear();
    guiUniformBuffers.clear();
    delete uniformBuffer;
    createUniformBuffers();

    uint32_t type_sz = MAX_FRAMES_IN_FLIGHT * this->textures.size();
    std::vector<VkDescriptorType> types(type_sz);
    for (size_t i = 0; i < textures.size(); i++) {
        types[2 * i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        types[2 * i + 1] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
    VkDescriptorType* tp = types.data();

    // TODO This is so fucking stupid I'm just going to prepare in advance a
    // big pool and then scale it up when needed
    pool = new ShaderDescriptorPool(device, tp, type_sz);
    uint32_t sz = textures.size() * MAX_FRAMES_IN_FLIGHT;
    if (sz == 0) sz = 1;
    uint32_t i[] = {sz, sz};
    VkDescriptorType type[] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    guiModule->recreateDescriptorPool(type, i, 2);
    renderDescriptorPool = new ShaderDescriptorPool(device, type, 2);
    createDescriptorSets();

    recreateSwapchain();
}

void Renderer::loadFinalShaders(std::string vShader, std::string gShader,
                                std::string fShader, VkDeviceSize UBOSize) {
    this->finalUBOSize = UBOSize;
    if (!vShader.empty()) this->finalVertShader = vShader;
    if (!gShader.empty()) this->finalGeomShader = gShader;
    if (!fShader.empty()) this->finalFragShader = fShader;

    device->wait();
    for (int i = 0; i < descriptorSets.size(); i++) delete descriptorSets[i];
    descriptorSets.clear();
    delete renderDescriptor;
    delete pool;
    delete renderDescriptorPool;

    delete uniformBuffer;

    uint32_t type_sz = MAX_FRAMES_IN_FLIGHT * this->textures.size();
    std::vector<VkDescriptorType> types(type_sz);
    for (size_t i = 0; i < textures.size(); i++) {
        types[2 * i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        types[2 * i + 1] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
    VkDescriptorType* tp = types.data();

    pool = new ShaderDescriptorPool(device, tp, type_sz);
    uint32_t sz = textures.size() * MAX_FRAMES_IN_FLIGHT;
    if (sz == 0) sz = 1;
    uint32_t i[] = {sz, sz};
    VkDescriptorType type[] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
    guiModule->recreateDescriptorPool(type, i, 2);
    renderDescriptorPool = new ShaderDescriptorPool(device, type, 2);

    uniformBuffer = new Buffer(&physicalDevice, device, finalUBOSize,
                               VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    createDescriptorSets();

    recreateSwapchain();
}

void Renderer::drawScreenCommandBuffer(CommandBuffer* commandBuffer,
                                       RenderPass* renderPass,
                                       Framebuffer* framebuffer) {
    commandBuffer->reset();
    commandBuffer->begin();

    std::vector<VkClearValue> clearValues = {VkClearValue(), VkClearValue()};
    clearValues[0].color = {{1.0f, 1.0f, 1.0f, 0.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    commandBuffer->beginRenderPass(renderPass, framebuffer, clearValues.data(),
                                   2);

    Model* lastModel = NULL;
    ImageView* lastTexture = NULL;
    std::vector<Entity*> entities = game->getEntities();
    if (!entities.empty()) {
        commandBuffer->bindPipeline(graphicsPipeline);

        commandBuffer->bindDescriptorSet(graphicsPipeline,
                                         descriptorSets[currentFrame]);
        for (Entity* e : entities) {
            Model* model = e->getModel();
            ImageView* texture = e->getTexture();
            if (lastModel != model) {
                model->toVertexBuffer()->copyTo(
                    vertexBuffer, device->getQueue("main"), commandPool);
                model->toIndicesBuffer()->copyTo(
                    indexBuffer, device->getQueue("main"), commandPool);
                commandBuffer->bindVertexBuffers(vertexBuffer, 1);
                commandBuffer->bindIndexBuffer(indexBuffer);
            }
            if (lastTexture != texture) {
                size_t idx = 0;
                for (idx = 0; idx < textures.size(); idx++) {
                    if (textures[idx] == texture) break;
                }
                commandBuffer->bindDescriptorSet(
                    graphicsPipeline, descriptorSets[idx * 2 + currentFrame]);
            }
            drawEntity(e, commandBuffer);
            lastModel = model;
            lastTexture = texture;
        }
    }

    commandBuffer->bindPipeline(guiModule->getPipeline());

    guiModel->toVertexBuffer()->copyTo(vertexBuffer, device->getQueue("main"),
                                       commandPool);
    guiModel->toIndicesBuffer()->copyTo(indexBuffer, device->getQueue("main"),
                                        commandPool);
    commandBuffer->bindVertexBuffers(vertexBuffer, 1);
    commandBuffer->bindIndexBuffer(indexBuffer);

    std::vector<Gui*> guis = game->getGuis();
    for (auto iter = guis.begin(); iter != guis.end(); iter++) {
        Gui* g = *iter;
        if (g->getRealPosition().x + g->getGraphicalPosition().x >
            2. + g->getSize().x)
            continue;
        ImageView* texture = g->getTexture();
        if (texture == NULL) texture = getTextureByName(textures, "missing");
        if (lastTexture != texture)
            commandBuffer->bindDescriptorSet(
                guiModule->getPipeline(),
                guiModule->getDescriptorSet(texture, currentFrame));

        drawGui(g, commandBuffer);
        lastTexture = texture;
    }

    commandBuffer->endRenderPass();

    commandBuffer->end();
}

void Renderer::recordCommandBuffer(CommandBuffer* commandBuffer,
                                   RenderPass* renderPass,
                                   Framebuffer* framebuffer) {
    commandBuffer->reset();
    commandBuffer->begin();

    std::vector<VkClearValue> clearValues = {VkClearValue(), VkClearValue()};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    commandBuffer->beginRenderPass(renderPass, framebuffer, clearValues.data(),
                                   2);

    commandBuffer->bindPipeline(renderPipeline);

    guiModel->toVertexBuffer()->copyTo(vertexBuffer, device->getQueue("main"),
                                       commandPool);
    guiModel->toIndicesBuffer()->copyTo(indexBuffer, device->getQueue("main"),
                                        commandPool);
    commandBuffer->bindVertexBuffers(vertexBuffer, 1);
    commandBuffer->bindIndexBuffer(indexBuffer);

    commandBuffer->bindDescriptorSet(renderPipeline, renderDescriptor);

    commandBuffer->draw(guiModel->getIndexes().size());

    commandBuffer->endRenderPass();

    commandBuffer->end();
}

void Renderer::drawEntity(Entity* entity, CommandBuffer* commandBuffer) {
    Model* model = entity->getModel();

    ShaderData* data = entity->getShaderData();
    commandBuffer->pushConstants(graphicsPipeline, VK_SHADER_STAGE_VERTEX_BIT,
                                 0, data->data, data->size);

    commandBuffer->draw(model->getIndexes().size());
    free(data->data);
    delete data;
}

void Renderer::drawGui(Gui* gui, CommandBuffer* commandBuffer) {
    Model* model = guiModel;

    ShaderData* data = gui->getShaderData();
    commandBuffer->pushConstants(guiModule->getPipeline(),
                                 VK_SHADER_STAGE_VERTEX_BIT, 0, data->data,
                                 data->size);

    commandBuffer->draw(model->getIndexes().size());
    free(data->data);
    delete data;
}

void Renderer::updateUniformBuffer(uint32_t currentImage) {
    double time_from_start = game->getCurrentTimeMicros() / 1000000.;
    float x = 0, y = 0;

    float ratio =
        swapchain->getExtent().width / (float)swapchain->getExtent().height;
    UniformBufferObject ubo{};
    ubo.view = glm::lookAt(glm::vec3(x, y, 1.0f), glm::vec3(x, y, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(90.0f),  // MASTER FOV
                                ratio, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;  // Invert y axis lul

    void* p = &ubo;

    void* data = NULL;
    if (!game->getEntities().empty()) {
        vkMapMemory(*(device->getDevice()),
                    *(uniformBuffers[currentImage]->getMemory()->getMemory()),
                    0, sizeof(ubo), 0, &data);
        memcpy(data, p, sizeof(ubo));
        vkUnmapMemory(
            *(device->getDevice()),
            *(uniformBuffers[currentImage]->getMemory()->getMemory()));
    }
    if (game->getGuis().empty()) return;
    ubo.view = glm::mat4(1.f);
    ubo.proj = glm::orthoLH_ZO<float>(-ratio, ratio, -1, 1, 0.f, 1.f);

    p = &ubo;
    size_t sz = game->updateUBO(p);
    vkMapMemory(*(device->getDevice()),
                *(guiUniformBuffers[currentImage]->getMemory()->getMemory()), 0,
                sz, 0, &data);
    memcpy(data, p, sz);
    vkUnmapMemory(*(device->getDevice()),
                  *(guiUniformBuffers[currentImage]->getMemory()->getMemory()));
    if (p != &ubo) game->freeUBO(p);
    ubo.view = glm::mat4(1.f);
    ubo.proj = glm::orthoLH_ZO<float>(-.5, .5, -.5, .5, 0.f, 1.f);
    p = &ubo;
    sz = game->updateFinalUBO(p);
    vkMapMemory(*(device->getDevice()),
                *(uniformBuffer->getMemory()->getMemory()), 0, sz, 0, &data);
    memcpy(data, p, sz);
    vkUnmapMemory(*(device->getDevice()),
                  *(uniformBuffer->getMemory()->getMemory()));
    if (p != &ubo) game->freeFinalUBO(p);
}

glm::vec2 Renderer::getVirtPos(glm::vec2 realPos) {
    float ratio =
        swapchain->getExtent().width / (float)swapchain->getExtent().height;
    glm::vec2 result = realPos;
    result.y /= swapchain->getExtent().height / 2.f;
    result.x /= swapchain->getExtent().width / 2.f / ratio;
    result.x -= ratio;
    result.y -= 1;
    return result;
}

void Renderer::drawFrame() {
    if (window->hasResized()) {
        window->setResized(false);
        recreateSwapchain();
    }
    inFlightFences[currentFrame]->wait();

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        *(device->getDevice()), *(swapchain->getSwapchain()), UINT64_MAX,
        *(imageAvailableSemaphores[currentFrame]->getSemaphore()),
        VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        std::cout << "outofdate";
        // recreateSwapchain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image!");

    updateUniformBuffer(currentFrame);

    inFlightFences[currentFrame]->reset();

    /*drawScreenCommandBuffer(
        commandBuffers[currentFrame], swapchain->getRenderPass(),
        swapchain->getFramebuffers()[imageIndex], swapchain->getExtent());*/
    // transitionImageLayout(renderImage, VK_IMAGE_LAYOUT_UNDEFINED,
    //                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    drawScreenCommandBuffer(renderCommandBuffer, renderPass, framebuffer);
    recordCommandBuffer(commandBuffers[currentFrame],
                        swapchain->getRenderPass(),
                        swapchain->getFramebuffers()[imageIndex]);

    renderCommandBuffer->submit(device->getQueue("main"));
    commandBuffers[currentFrame]->submit(
        device->getQueue("main"), imageAvailableSemaphores[currentFrame],
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
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

    result = vkQueuePresentKHR(*(device->getQueue("main")->getQueue()),
                               &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        window->hasResized()) {
        window->setResized(false);
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

bool Renderer::isDeviceSuitable(VkPhysicalDevice device) {
    std::map<std::string, FamilyPredicate> familyPredicates = {
        {"main",
         [](VkPhysicalDevice dev, VkQueueFamilyProperties prop, uint32_t id) {
             return prop.queueFlags & VK_QUEUE_GRAPHICS_BIT;
         }},
        {"secondary",
         [this](VkPhysicalDevice dev, VkQueueFamilyProperties prop,
                uint32_t id) {
             VkBool32 presentSupport = false;
             vkGetPhysicalDeviceSurfaceSupportKHR(dev, id, *surface,
                                                  &presentSupport);
             return presentSupport;
         }},
        {"compute",
         [](VkPhysicalDevice dev, VkQueueFamilyProperties prop, uint32_t id) {
             return prop.queueFlags & VK_QUEUE_COMPUTE_BIT;
         }}};
    std::vector<VkQueueFamilyProperties> fam;  // Hey fam
    getQueueFamilies(device, fam);
    std::map<std::string, FamilyData> families =
        findQueueFamilies(device, fam, familyPredicates);

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

    for (auto entry : families) delete entry.second.properties;

    return families.size() == familyPredicates.size() && extensionsSupported &&
           swapChainAdequate && supportedFeatures.samplerAnisotropy &&
           supportedFeatures.geometryShader;
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

    if (glfwExtensions == NULL)
        std::cerr << "GLFW API unavailable" << std::endl;

    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}
