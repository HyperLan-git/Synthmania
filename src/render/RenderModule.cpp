#include "RenderModule.hpp"

RenderModule::RenderModule(Instance* instance, VkPhysicalDevice* physicalDevice,
                           Device* device, std::string name, uint32_t w,
                           uint32_t h, RenderPass* renderPass,
                           VkDescriptorSetLayoutBinding* bindings,
                           uint32_t* nDescriptorSets, uint32_t nBindings,
                           VkPipelineShaderStageCreateInfo* shaderStages,
                           uint32_t nShaders,
                           VkPushConstantRange* constantRanges,
                           uint32_t nConstantRange) {
    this->name = name;
    this->descriptorBindings = new VkDescriptorSetLayoutBinding[nBindings];
    for (int i = 0; i < nBindings; i++)
        this->descriptorBindings[i] = bindings[i];
    this->nDescriptorBindings = nBindings;
    this->shaderStages = new VkPipelineShaderStageCreateInfo[nShaders];
    this->nShaders = nShaders;
    for (int i = 0; i < nShaders; i++) this->shaderStages[i] = shaderStages[i];
    this->constantRanges = new VkPushConstantRange[nConstantRange];
    this->nConstantRange = nConstantRange;
    for (int i = 0; i < nConstantRange; i++)
        this->constantRanges[i] = constantRanges[i];
    this->extent = {w, h};

    this->instance = instance;
    this->physicalDevice = *physicalDevice;
    this->device = device;

    DebugFunc functions = getDebugFunctions(instance);

    renderImage = new Image(
        physicalDevice, device, w, h, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    setName(functions, device, name + " image", VK_OBJECT_TYPE_IMAGE,
            renderImage->getImage());
    renderImageView =
        new ImageView(device, renderImage, VK_FORMAT_R8G8B8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT, name + " view");

    VkFormat depthFormat = findDepthFormat(*physicalDevice);
    depthImage = new Image(physicalDevice, device, w, h, depthFormat,
                           VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    setName(functions, device, name + " depth image", VK_OBJECT_TYPE_IMAGE,
            depthImage->getImage());
    depthImageView = new ImageView(device, depthImage, depthFormat,
                                   VK_IMAGE_ASPECT_DEPTH_BIT, "depth image");
    setName(functions, device, name + " depth image view",
            VK_OBJECT_TYPE_IMAGE_VIEW, depthImageView->getView());

    sampler = new TextureSampler(physicalDevice, device);
    setName(functions, device, name + " image sampler", VK_OBJECT_TYPE_SAMPLER,
            sampler->getSampler());

    commandPool = new CommandPool(device);
    commandBuffer = new CommandBuffer(device, commandPool, false);
    setName(functions, device, name + " command buffer",
            VK_OBJECT_TYPE_COMMAND_BUFFER, *(commandBuffer->getBuffer()));

    renderLayout = new ShaderDescriptorSetLayout(device, bindings, nBindings);
    setName(functions, device, name + " layout",
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, *(renderLayout->getLayout()));

    renderPipelineLayout = new PipelineLayout(device, renderLayout,
                                              nConstantRange, constantRanges);
    setName(functions, device, name + " pipeline layout",
            VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            *(renderPipelineLayout->getLayout()));

    renderPipeline = new Pipeline(device, renderPipelineLayout, renderPass,
                                  shaderStages, nShaders, extent);
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            *(renderPipeline->getPipeline()));

    VkDescriptorType types[nBindings];
    for (int i = 0; i < nBindings; i++) types[i] = bindings[i].descriptorType;
    this->descriptorPool =
        new ShaderDescriptorPool(device, types, nDescriptorSets, nBindings);
    setName(functions, device, name + " descriptor pool",
            VK_OBJECT_TYPE_DESCRIPTOR_POOL, *(this->descriptorPool->getPool()));
    this->renderPass = renderPass;
}

Pipeline* RenderModule::getPipeline() { return this->renderPipeline; }

ShaderDescriptorSet* RenderModule::getDescriptorSet(ImageView* texture,
                                                    size_t current) {
    return descriptorSets[texture][current];
}

ShaderDescriptorSet* RenderModule::addDescriptorSet(ImageView* img,
                                                    Buffer* buffer) {
    DebugFunc functions = getDebugFunctions(instance);
    ShaderDescriptorSet* descriptor = new ShaderDescriptorSet(
        device, this->descriptorPool, this->renderLayout);
    updateDescriptorSet(descriptor, img, sampler, buffer);
    auto iter = this->descriptorSets.find(img);
    if (iter == this->descriptorSets.end()) {
        std::vector<ShaderDescriptorSet*> set = {descriptor};
        this->descriptorSets.emplace(img, set);
        setName(functions, device, name + img->getName(),
                VK_OBJECT_TYPE_DESCRIPTOR_SET, *(descriptor->getSet()));
    } else {
        setName(functions, device,
                name + img->getName() + std::to_string(iter->second.size()),
                VK_OBJECT_TYPE_DESCRIPTOR_SET, *(descriptor->getSet()));
        iter->second.push_back(descriptor);
    }
    return descriptor;
}

void RenderModule::setExtent(uint32_t w, uint32_t h) {
    this->extent = {w, h};
    delete renderImageView;
    delete renderImage;
    delete depthImageView;
    delete depthImage;

    delete sampler;

    delete renderPipeline;

    DebugFunc functions = getDebugFunctions(instance);

    renderImage = new Image(
        &physicalDevice, device, w, h, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    setName(functions, device, name + " image", VK_OBJECT_TYPE_IMAGE,
            renderImage->getImage());
    renderImageView =
        new ImageView(device, renderImage, VK_FORMAT_R8G8B8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT, name + " view");
    VkFormat depthFormat = findDepthFormat(physicalDevice);
    depthImage = new Image(&physicalDevice, device, w, h, depthFormat,
                           VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    setName(functions, device, name + " depth image", VK_OBJECT_TYPE_IMAGE,
            depthImage->getImage());
    depthImageView = new ImageView(device, depthImage, depthFormat,
                                   VK_IMAGE_ASPECT_DEPTH_BIT, "depth image");
    setName(functions, device, name + " depth image view",
            VK_OBJECT_TYPE_IMAGE_VIEW, depthImageView->getView());

    sampler = new TextureSampler(&physicalDevice, device);
    setName(functions, device, name + " image sampler", VK_OBJECT_TYPE_SAMPLER,
            sampler->getSampler());

    renderPipeline = new Pipeline(device, renderPipelineLayout, renderPass,
                                  shaderStages, nShaders, VkExtent2D({w, h}));
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            *(renderPipeline->getPipeline()));
    std::vector<VkDescriptorType> types;
    for (int i = 0; i < nDescriptorBindings; i++)
        types.push_back(descriptorBindings[i].descriptorType);
    descriptorPool =
        new ShaderDescriptorPool(device, types.data(), nDescriptorBindings);
}

void RenderModule::setShaders(VkPipelineShaderStageCreateInfo* shaderStages,
                              uint32_t nShaders) {
    delete[] this->shaderStages;
    this->nShaders = nShaders;
    this->shaderStages = new VkPipelineShaderStageCreateInfo[nShaders];
    for (int i = 0; i < nShaders; i++) this->shaderStages[i] = shaderStages[i];

    delete renderPipeline;

    DebugFunc functions = getDebugFunctions(instance);

    renderPipeline = new Pipeline(device, renderPipelineLayout, renderPass,
                                  this->shaderStages, nShaders,
                                  this->framebuffer->getExtent());
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            *(renderPipeline->getPipeline()));
}

void RenderModule::recreateDescriptorPool(VkDescriptorType* types,
                                          uint32_t* nDescriptorSets,
                                          uint32_t nTypes) {
    for (auto entry : descriptorSets)
        for (ShaderDescriptorSet* descriptor : entry.second) delete descriptor;
    descriptorSets.clear();
    delete descriptorPool;

    DebugFunc functions = getDebugFunctions(instance);
    this->descriptorPool =
        new ShaderDescriptorPool(device, types, nDescriptorSets, nTypes);
    setName(functions, device, name + " descriptor pool",
            VK_OBJECT_TYPE_DESCRIPTOR_POOL, *(this->descriptorPool->getPool()));
}

RenderModule::~RenderModule() {
    delete[] descriptorBindings;
    delete[] shaderStages;
    delete[] constantRanges;
    delete renderImageView;
    delete renderImage;
    delete depthImageView;
    delete depthImage;
    delete sampler;
    delete framebuffer;
    delete renderPipeline;
    delete renderLayout;
    delete renderPipelineLayout;
    for (auto entry : descriptorSets)
        for (ShaderDescriptorSet* descriptor : entry.second) delete descriptor;
    delete descriptorPool;

    delete commandBuffer;
    delete commandPool;
}

void updateDescriptorSet(ShaderDescriptorSet* descriptor, ImageView* texture,
                         TextureSampler* sampler, Buffer* uniformBuffer) {
    VkDescriptorImageInfo imageInfo = sampler->createImageInfo(texture);
    VkDescriptorBufferInfo bufferInfo = uniformBuffer->createBufferInfo();

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 1,
                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, NULL,
                             &imageInfo);
    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo,
                             NULL);
}