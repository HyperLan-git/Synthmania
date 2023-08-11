#include "RenderModule.hpp"

RenderModule::RenderModule(
    Instance& instance, Device& device, std::string name, uint32_t w,
    uint32_t h, RenderPass& renderPass,
    std::initializer_list<VkDescriptorSetLayoutBinding> bindings,
    std::initializer_list<VkPipelineShaderStageCreateInfo> shaders,
    std::initializer_list<VkPushConstantRange> constantRanges,
    std::initializer_list<VkDescriptorPoolSize> sets)
    : device(device),
      instance(instance),
      renderPass(renderPass),
      name(name),
      descriptorBindings(bindings),
      shaderStages(shaders),
      constantRanges(constantRanges),
      descriptorPoolSizes(sets),
      extent({w, h}) {
    DebugFunc functions = getDebugFunctions(instance);

    renderImageView = std::make_shared<ImageView>(
        std::make_shared<Image>(
            device, w, h, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, name + " view");

    VkFormat depthFormat = findDepthFormat(device.getPhysicalDevice());
    depthImageView = std::make_shared<ImageView>(
        std::make_shared<Image>(device, w, h, depthFormat,
                                VK_IMAGE_TILING_OPTIMAL,
                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, "depth image");
    setName(functions, device, name + " depth image view",
            VK_OBJECT_TYPE_IMAGE_VIEW, depthImageView->getView());

    sampler = new TextureSampler(device);
    setName(functions, device, name + " image sampler", VK_OBJECT_TYPE_SAMPLER,
            sampler->getSampler());

    commandPool = new CommandPool(device);
    commandBuffer = new CommandBuffer(*commandPool, false);
    setName(functions, device, name + " command buffer",
            VK_OBJECT_TYPE_COMMAND_BUFFER, commandBuffer->getBuffer());

    renderLayout = new ShaderDescriptorSetLayout(device, descriptorBindings);
    setName(functions, device, name + " layout",
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, renderLayout->getLayout());

    renderPipelineLayout = new PipelineLayout(*renderLayout, constantRanges);
    setName(functions, device, name + " pipeline layout",
            VK_OBJECT_TYPE_PIPELINE_LAYOUT, renderPipelineLayout->getLayout());

    renderPipeline =
        new Pipeline(*renderPipelineLayout, renderPass, extent, shaderStages);
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            renderPipeline->getPipeline());

    this->descriptorPool = new ShaderDescriptorPool(device, sets);
    setName(functions, device, name + " descriptor pool",
            VK_OBJECT_TYPE_DESCRIPTOR_POOL, this->descriptorPool->getPool());
}

Pipeline* RenderModule::getPipeline() { return this->renderPipeline; }

ShaderDescriptorSet* RenderModule::getDescriptorSet(TexPtr texture,
                                                    size_t current) {
    return descriptorSets[texture->getName()][current];
}

ShaderDescriptorSet* RenderModule::addDescriptorSet(TexPtr img,
                                                    Buffer* buffer) {
    DebugFunc functions = getDebugFunctions(instance);
    ShaderDescriptorSet* descriptor =
        new ShaderDescriptorSet(*this->descriptorPool, *this->renderLayout);
    updateDescriptorSet(*descriptor, *img, *sampler, *buffer);
    auto iter = this->descriptorSets.find(img->getName());
    if (iter == this->descriptorSets.end()) {
        std::vector<ShaderDescriptorSet*> set = {descriptor};
        this->descriptorSets.emplace(img->getName(), set);
        setName(functions, device, name + img->getName(),
                VK_OBJECT_TYPE_DESCRIPTOR_SET, descriptor->getSet());
    } else {
        setName(functions, device,
                name + img->getName() + std::to_string(iter->second.size()),
                VK_OBJECT_TYPE_DESCRIPTOR_SET, descriptor->getSet());
        iter->second.push_back(descriptor);
    }
    return descriptor;
}

void RenderModule::setExtent(uint32_t w, uint32_t h) {
    this->extent = {w, h};
    renderImageView.reset();
    depthImageView.reset();

    delete sampler;

    delete renderPipeline;

    DebugFunc functions = getDebugFunctions(instance);

    renderImageView = std::make_shared<ImageView>(
        std::make_shared<Image>(
            device, w, h, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, name + " view");
    VkFormat depthFormat = findDepthFormat(device.getPhysicalDevice());
    depthImageView = std::make_shared<ImageView>(
        std::make_shared<Image>(device, w, h, depthFormat,
                                VK_IMAGE_TILING_OPTIMAL,
                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, "depth image");
    setName(functions, device, name + " depth image view",
            VK_OBJECT_TYPE_IMAGE_VIEW, depthImageView->getView());

    sampler = new TextureSampler(device);
    setName(functions, device, name + " image sampler", VK_OBJECT_TYPE_SAMPLER,
            sampler->getSampler());

    renderPipeline = new Pipeline(*renderPipelineLayout, renderPass,
                                  VkExtent2D({w, h}), shaderStages);
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            renderPipeline->getPipeline());
    descriptorPool = new ShaderDescriptorPool(device, descriptorPoolSizes);
}

void RenderModule::setShaders(
    std::initializer_list<VkPipelineShaderStageCreateInfo> shaderStages) {
    this->shaderStages = shaderStages;

    delete renderPipeline;

    DebugFunc functions = getDebugFunctions(instance);

    renderPipeline =
        new Pipeline(*renderPipelineLayout, renderPass,
                     this->framebuffer->getExtent(), this->shaderStages);
    setName(functions, device, name + " pipeline", VK_OBJECT_TYPE_PIPELINE,
            renderPipeline->getPipeline());
}

void RenderModule::recreateDescriptorPool(
    std::initializer_list<VkDescriptorPoolSize> sets) {
    for (auto entry : descriptorSets)
        for (ShaderDescriptorSet* descriptor : entry.second) delete descriptor;
    descriptorSets.clear();
    delete descriptorPool;

    DebugFunc functions = getDebugFunctions(instance);
    descriptorPoolSizes = sets;
    this->descriptorPool =
        new ShaderDescriptorPool(device, descriptorPoolSizes);
    setName(functions, device, name + " descriptor pool",
            VK_OBJECT_TYPE_DESCRIPTOR_POOL, this->descriptorPool->getPool());
}

RenderModule::~RenderModule() {
    renderImageView.reset();
    depthImageView.reset();
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

void updateDescriptorSet(ShaderDescriptorSet& descriptor, ImageView& texture,
                         TextureSampler& sampler, Buffer& uniformBuffer) {
    VkDescriptorImageInfo imageInfo = sampler.createImageInfo(texture);
    VkDescriptorBufferInfo bufferInfo = uniformBuffer.createBufferInfo();

    descriptor.updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 1,
                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, NULL,
                            &imageInfo);
    descriptor.updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
                            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo,
                            NULL);
}