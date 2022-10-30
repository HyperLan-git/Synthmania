#include "RenderModule.hpp"

RenderModule::RenderModule(
    Instance* instance, VkPhysicalDevice* physicalDevice, Device* device,
    std::string name, uint32_t w, uint32_t h, RenderPass* renderPass,
    VkDescriptorSetLayoutBinding* bindings, uint32_t nBindings,
    VkPipelineShaderStageCreateInfo* shaderStages, uint32_t nShaders,
    VkPushConstantRange* constantRanges, uint32_t nConstantRange) {
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
            *(renderImage->getImage()));
    renderImageView =
        new ImageView(device, renderImage, VK_FORMAT_R8G8B8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT, name + " view");

    VkFormat depthFormat = findDepthFormat(*physicalDevice);
    depthImage = new Image(physicalDevice, device, w, h, depthFormat,
                           VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    setName(functions, device, name + " depth image", VK_OBJECT_TYPE_IMAGE,
            *(depthImage->getImage()));
    depthImageView = new ImageView(device, depthImage, depthFormat,
                                   VK_IMAGE_ASPECT_DEPTH_BIT, "depth image");
    setName(functions, device, name + " depth image view",
            VK_OBJECT_TYPE_IMAGE_VIEW, *(depthImageView->getView()));

    sampler = new TextureSampler(physicalDevice, device);
    setName(functions, device, name + " image sampler", VK_OBJECT_TYPE_SAMPLER,
            *(sampler->getSampler()));

    commandPool = new CommandPool(device);
    commandBuffer = new CommandBuffer(device, commandPool, false);
    setName(functions, device, name + " command buffer",
            VK_OBJECT_TYPE_COMMAND_BUFFER, *(renderLayout->getLayout()));

    renderLayout = new ShaderDescriptorSetLayout(device, bindings, nBindings);
    setName(functions, device, name + " layout",
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, *(renderLayout->getLayout()));

    renderPipelineLayout = new PipelineLayout(device, renderLayout,
                                              nConstantRange, constantRanges);
    setName(functions, device, name + " pipeline layout",
            VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            *(renderPipelineLayout->getLayout()));

    renderPipeline = new Pipeline(device, renderPipelineLayout, renderPass,
                                  shaderStages, nShaders, VkExtent2D({w, h}));
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            *(renderPipeline->getPipeline()));

    VkDescriptorType types[nBindings];
    for (int i = 0; i < nBindings; i++) types[i] = bindings[i].descriptorType;
    renderDescriptorPool = new ShaderDescriptorPool(device, types, nBindings);
    renderDescriptor =
        new ShaderDescriptorSet(device, renderDescriptorPool, renderLayout);
    setName(functions, device, name + " descriptor",
            VK_OBJECT_TYPE_DESCRIPTOR_SET, *(renderDescriptor->getSet()));
    // TODO arbitrary bindings
    updateDescriptorSet(renderDescriptor, uniformBuffer, 0);
    updateDescriptorSet(renderDescriptor, renderImageView, sampler, 1);
    renderPass = new RenderPass(physicalDevice, device, VK_FORMAT_B8G8R8A8_SRGB,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    setName(functions, device, name + " render pass",
            VK_OBJECT_TYPE_RENDER_PASS, *(renderPass->getPass()));
}

void RenderModule::setExtent(uint32_t w, uint32_t h) {
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
            *(renderImage->getImage()));
    renderImageView =
        new ImageView(device, renderImage, VK_FORMAT_R8G8B8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT, name + " view");
    VkFormat depthFormat = findDepthFormat(physicalDevice);
    depthImage = new Image(&physicalDevice, device, w, h, depthFormat,
                           VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    setName(functions, device, name + " depth image", VK_OBJECT_TYPE_IMAGE,
            *(depthImage->getImage()));
    depthImageView = new ImageView(device, depthImage, depthFormat,
                                   VK_IMAGE_ASPECT_DEPTH_BIT, "depth image");
    setName(functions, device, name + " depth image view",
            VK_OBJECT_TYPE_IMAGE_VIEW, *(depthImageView->getView()));

    sampler = new TextureSampler(&physicalDevice, device);
    setName(functions, device, name + " image sampler", VK_OBJECT_TYPE_SAMPLER,
            *(sampler->getSampler()));

    renderPipeline = new Pipeline(device, renderPipelineLayout, renderPass,
                                  shaderStages, nShaders, VkExtent2D({w, h}));
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            *(renderPipeline->getPipeline()));

    renderDescriptor->resetAccess();
    updateDescriptorSet(renderDescriptor, uniformBuffer, 0);
    updateDescriptorSet(renderDescriptor, renderImageView, sampler, 1);
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

RenderModule::~RenderModule() {
    delete[] descriptorBindings;
    delete[] shaderStages;
    delete[] constantRanges;
    delete renderImageView;
    delete renderImage;
    delete depthImageView;
    delete depthImage;
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
}

void updateDescriptorSet(ShaderDescriptorSet* descriptor, ImageView* texture,
                         TextureSampler* sampler, int binding) {
    VkDescriptorImageInfo* imageInfo = createImageInfo(texture, sampler);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, binding,
                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, NULL,
                             imageInfo);

    delete imageInfo;
}

void updateDescriptorSet(ShaderDescriptorSet* descriptor, Buffer* uniformBuffer,
                         int binding) {
    VkDescriptorBufferInfo* bufferInfo = createBufferInfo(uniformBuffer);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, binding,
                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, bufferInfo,
                             NULL);

    delete bufferInfo;
}