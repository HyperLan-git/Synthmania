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
    createInstance();
    surface = instance->createSurface(window);
    pickPhysicalDevice();
    createLogicalDevice();
    swapchain = new Swapchain(device, &physicalDevice, window, surface);
    createGraphicsPipeline();
    createGuiPipeline();
    commandPool = new CommandPool(physicalDevice, device);
    guiModel = new Model({{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
                          {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
                          {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
                          {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}}},
                         {3, 0, 2, 0, 1, 2}, &physicalDevice, device);
    guiSampler = new TextureSampler(&physicalDevice, device);
    textureSampler = new TextureSampler(&physicalDevice, device);
    Model* model =
        new Model("resources/models/room.obj", &physicalDevice, device);
    models.push_back(model);
    createVertexBuffer(model->toVertexBuffer()->getSize());
    createIndexBuffer(model->toIndicesBuffer()->getSize());
    createUniformBuffers();

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        commandBuffers.push_back(new CommandBuffer(device, commandPool, false));

        imageAvailableSemaphores.push_back(new Semaphore(device));
        renderFinishedSemaphores.push_back(new Semaphore(device));
        inFlightFences.push_back(new Fence(device));
    }
}

Image* Renderer::loadCharacter(FT_Face face, ulong character) {
    FT_GlyphSlot glyphSlot = face->glyph;
    FT_UInt i = FT_Get_Char_Index(face, character);
    FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
    FT_Render_Glyph(glyphSlot, FT_RENDER_MODE_NORMAL);
    uint w = glyphSlot->bitmap.width + 2, h = glyphSlot->bitmap.rows + 2;
    uint8_t buffer[w * h * 4] = {0};

    uint8_t* bitmap = glyphSlot->bitmap.buffer;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (y == 0 || y == h - 1 || x == 0 || x == w - 1) {
                buffer[(x + y * w) * 4 + 3] = 0;
                continue;
            }
            uint8_t value = bitmap[(x - 1) + (y - 1) * (w - 2)];
            buffer[(x + y * w) * 4 + 3] = value;
        }
    }

    Image* image =
        new Image(&physicalDevice, device, w, h, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_LINEAR,  // Fricking tiling
                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    void* d;
    vkMapMemory(*(device->getDevice()), *(image->getMemory()->getMemory()), 0,
                w * h * 4, 0, &d);
    memcpy(d, buffer, w * h * 4);
    vkUnmapMemory(*(device->getDevice()), *(image->getMemory()->getMemory()));
    return image;
}

void Renderer::loadFonts(
    std::map<std::string, std::vector<ulong>> fontsToLoad) {
    FT_Library* lib = new FT_Library();
    FT_Init_FreeType(lib);
    for (auto entry : fontsToLoad) {
        FT_Face f;
        if (FT_New_Face(*lib, entry.first.c_str(), 0, &f)) continue;
        if (FT_Set_Pixel_Sizes(f, 0, static_cast<FT_UInt>(FONT_SIZE))) continue;

        Font font;
        font.name = f->family_name;

        for (auto c : entry.second) {
            std::string charName = f->family_name;
            charName.append("_");
            charName.append(std::to_string(c));
            addTexture(loadCharacter(f, c), charName.c_str());
            Character chr;
            chr.texture = getTextureByName(textures, charName.c_str());
            chr.character = c;
            chr.width = f->glyph->bitmap.width + 2;
            chr.height = f->glyph->bitmap.rows + 2;
            chr.advance = f->glyph->linearHoriAdvance / 65536;
            chr.offsetTop = f->glyph->bitmap_top;
            chr.offsetLeft = f->glyph->bitmap_left;
            font.characters.emplace(c, chr);
        }
        fonts.push_back(font);
    }
}

std::vector<Font> Renderer::getFonts() { return fonts; }

Character Renderer::getCharacter(std::string fontName, ulong code) {
    for (Font f : fonts) {
        if (fontName.compare(f.name) != 0) continue;
        return f.characters[code];
    }
    return Character({0, 0, 0, 0, 0, 0, NULL});
}

void Renderer::loadTextures(std::map<std::string, std::string> textures,
                            std::map<std::string, std::vector<ulong>> fonts) {
    for (auto entry : textures) {
        this->textures.push_back(
            readTexture(entry.second.c_str(), entry.first.c_str()));
    }
    loadFonts(fonts);

    uint32_t type_sz = MAX_FRAMES_IN_FLIGHT * this->textures.size();
    std::vector<VkDescriptorType> types(type_sz);
    for (size_t i = 0; i < textures.size(); i++) {
        types[2 * i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        types[2 * i + 1] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
    VkDescriptorType* tp = types.data();

    pool = new ShaderDescriptorPool(device, tp, type_sz);
    guiPool = new ShaderDescriptorPool(device, tp, type_sz);
    createDescriptorSets();
}

VkPhysicalDevice* Renderer::getPhysicalDevice() { return &physicalDevice; }
Device* Renderer::getDevice() { return device; }

void Renderer::setStartTime(double start) { this->startTime = start; }

std::vector<ImageView*> Renderer::getTextures() { return textures; }

void Renderer::render() { drawFrame(); }

Renderer::~Renderer() {
    device->wait();

    delete swapchain;
    delete graphicsPipelineLayout;
    delete graphicsPipeline;
    delete guiPipelineLayout;
    delete guiPipeline;

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        delete constantUniformBuffers[i];
        delete uniformBuffers[i];
        delete guiConstantUniformBuffers[i];
        delete guiUniformBuffers[i];
    }

    delete pool;
    delete guiPool;

    delete textureSampler;
    delete guiSampler;

    delete shaderLayout;
    delete guiShaderLayout;

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

    delete commandPool;

    delete device;

    instance->destroySurface(surface);
    delete instance;
}

void Renderer::recreateSwapchain() {
    uint32_t width = 0, height = 0;
    window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0) {
        window->getFramebufferSize(&width, &height);
        glfwWaitEvents();
    }

    device->wait();

    delete swapchain;
    delete graphicsPipeline;
    delete graphicsPipelineLayout;
    delete guiPipeline;
    delete guiPipelineLayout;

    delete shaderLayout;
    delete guiShaderLayout;

    swapchain = new Swapchain(device, &physicalDevice, window, surface);
    createGraphicsPipeline();
    createGuiPipeline();
}

void Renderer::createInstance() {
    uint32_t ver = VK_MAKE_VERSION(1, 0, 0);
    const char *appName = "Synthmania", *engineName = "No Engine";
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

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShader->toPipeline(),
                                                      fragShader->toPipeline()};

    graphicsPipelineLayout = new PipelineLayout(device, shaderLayout, 1, range);

    graphicsPipeline =
        new Pipeline(device, graphicsPipelineLayout, swapchain->getRenderPass(),
                     shaderStages, 2, swapchain->getExtent());
    delete range;
    delete vertShader;
    delete fragShader;
}

void Renderer::createGuiPipeline() {
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
    guiShaderLayout = new ShaderDescriptorSetLayout(device, bindings, 2);

    auto vertShaderCode = readFile("bin/vert_gui.spv");
    auto fragShaderCode = readFile("bin/frag.spv");

    Shader* vertShader = new Shader("main", device, vertShaderCode, true);
    Shader* fragShader = new Shader("main", device, fragShaderCode, false);
    VkPushConstantRange* range = new VkPushConstantRange();
    range->offset = 0;
    range->size = sizeof(GuiData);
    range->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShader->toPipeline(),
                                                      fragShader->toPipeline()};

    guiPipelineLayout = new PipelineLayout(device, guiShaderLayout, 1, range);

    guiPipeline =
        new Pipeline(device, guiPipelineLayout, swapchain->getRenderPass(),
                     shaderStages, 2, swapchain->getExtent());
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

void Renderer::addTexture(Image* texture, const char* name) {
    this->textures.push_back(new ImageView(device, texture,
                                           VK_FORMAT_R8G8B8A8_SRGB,
                                           VK_IMAGE_ASPECT_COLOR_BIT, name));
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

ImageView* Renderer::readTexture(const char* path, const char* name) {
    Image* tex = createTextureImage(path);

    return new ImageView(device, tex, VK_FORMAT_R8G8B8A8_SRGB,
                         VK_IMAGE_ASPECT_COLOR_BIT, name);
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

void Renderer::convertImage(Image* src, VkImageLayout srcImageLayout,
                            Image* dst, VkImageLayout dstImageLayout,
                            VkFilter filter) {
    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    commandBuffer->begin();

    commandBuffer->convertImage(src, srcImageLayout, dst, dstImageLayout,
                                filter);

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
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers.push_back(
            new Buffer(&physicalDevice, device, bufferSize,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        constantUniformBuffers.push_back(
            new Buffer(&physicalDevice, device, sizeof(EntityData),
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        guiUniformBuffers.push_back(
            new Buffer(&physicalDevice, device, bufferSize,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        guiConstantUniformBuffers.push_back(
            new Buffer(&physicalDevice, device, sizeof(GuiData),
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    }
}

void Renderer::updateDescriptorSet(ShaderDescriptorSet* descriptor,
                                   ImageView* texture, TextureSampler* sampler,
                                   Buffer* uniformBuffer) {
    VkDescriptorBufferInfo* bufferInfo = createBufferInfo(uniformBuffer);

    VkDescriptorImageInfo* imageInfo = createImageInfo(texture, sampler);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, bufferInfo,
                             nullptr);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 1,
                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr,
                             imageInfo);

    delete imageInfo;
    delete bufferInfo;
}

void Renderer::createDescriptorSets() {
    size_t j = 0;
    for (ImageView* img : textures) {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            descriptorSets.push_back(
                new ShaderDescriptorSet(device, pool, shaderLayout));

            guiDescriptorSets.push_back(
                new ShaderDescriptorSet(device, guiPool, guiShaderLayout));

            // God this is so cursed why
            updateDescriptorSet(descriptorSets[j + i], img, textureSampler,
                                uniformBuffers[i]);
            updateDescriptorSet(guiDescriptorSets[j + i], img, guiSampler,
                                guiUniformBuffers[i]);
        }
        j += 2;
    }
}

void Renderer::recordCommandBuffer(CommandBuffer* commandBuffer,
                                   uint32_t imageIndex) {
    commandBuffer->reset();
    commandBuffer->begin();

    std::vector<VkClearValue> clearValues = {VkClearValue(), VkClearValue()};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    commandBuffer->beginRenderPass(swapchain, imageIndex, clearValues.data(),
                                   2);

    Model* lastModel = nullptr;
    ImageView* lastTexture = nullptr;
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
                    vertexBuffer, device->getQueue(0), commandPool);
                model->toIndicesBuffer()->copyTo(
                    indexBuffer, device->getQueue(0), commandPool);
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

    commandBuffer->bindPipeline(guiPipeline);

    guiModel->toVertexBuffer()->copyTo(vertexBuffer, device->getQueue(0),
                                       commandPool);
    guiModel->toIndicesBuffer()->copyTo(indexBuffer, device->getQueue(0),
                                        commandPool);
    commandBuffer->bindVertexBuffers(vertexBuffer, 1);
    commandBuffer->bindIndexBuffer(indexBuffer);

    commandBuffer->bindDescriptorSet(guiPipeline,
                                     guiDescriptorSets[currentFrame]);

    std::vector<Gui*> guis = game->getGuis();
    for (auto iter = guis.begin(); iter != guis.end(); iter++) {
        Gui* g = *iter;
        ImageView* texture = g->getTexture();
        if (texture == NULL) {
            throw std::runtime_error("No textures ???");
        }
        if (lastTexture != texture) {
            size_t idx = 0;
            for (idx = 0; idx < textures.size(); idx++) {
                if (textures[idx] == texture) break;
            }
            if (idx >= textures.size()) {
                std::string err = "Texture ";
                err.append(texture->getName());
                err.append(" not found !");
                throw std::runtime_error(err);
            }
            commandBuffer->bindDescriptorSet(
                guiPipeline, guiDescriptorSets[idx * 2 + currentFrame]);
        }
        drawGui(g, commandBuffer);
        lastTexture = texture;
    }

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
    commandBuffer->pushConstants(guiPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                 data->data, data->size);

    commandBuffer->draw(model->getIndexes().size());
    free(data->data);
    delete data;
}

void Renderer::updateUniformBuffer(uint32_t currentImage) {
    double time_from_start = game->getCurrentTimeMicros() / 1000000.;
    float x = cos(time_from_start * 2 / 3) / 2,
          y = sin(time_from_start * 5 / 3) / 2;  // Lissajous :)

    x = y = 0;
    float ratio =
        swapchain->getExtent().width / (float)swapchain->getExtent().height;
    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(glm::vec3(x, y, 1.0f), glm::vec3(x, y, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(90.0f),  // MASTER FOV
                                ratio, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;  // Invert y axis cause I like my y axis positive up

    void *data = NULL, *data2 = NULL;
    vkMapMemory(*(device->getDevice()),
                *(uniformBuffers[currentImage]->getMemory()->getMemory()), 0,
                sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(*(device->getDevice()),
                  *(uniformBuffers[currentImage]->getMemory()->getMemory()));

    ubo.model = glm::mat4(1.f);
    ubo.view = glm::mat4(1.f);
    ubo.proj = glm::orthoLH_ZO<float>(-ratio, ratio, -1, 1, 0.f, 1.f);

    vkMapMemory(*(device->getDevice()),
                *(guiUniformBuffers[currentImage]->getMemory()->getMemory()), 0,
                sizeof(ubo), 0, &data2);
    memcpy(data2, &ubo, sizeof(ubo));
    vkUnmapMemory(*(device->getDevice()),
                  *(guiUniformBuffers[currentImage]->getMemory()->getMemory()));
}

void Renderer::drawFrame() {
    inFlightFences[currentFrame]->wait();

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        *(device->getDevice()), *(swapchain->getSwapchain()), UINT64_MAX,
        *(imageAvailableSemaphores[currentFrame]->getSemaphore()),
        VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
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
        recreateSwapchain();
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