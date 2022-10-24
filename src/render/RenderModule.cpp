#include "RenderModule.hpp"

RenderModule::RenderModule(Instance* instance, VkPhysicalDevice* physicalDevice,
                           Device* device, std::string name, uint32_t w,
                           uint32_t h, RenderPass* renderPass,
                           VkDescriptorSetLayoutBinding* bindings,
                           uint32_t nBindings,
                           VkPipelineShaderStageCreateInfo* shaderStages,
                           uint32_t nShaders) {
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

    renderPipelineLayout = new PipelineLayout(device, renderLayout, 0, NULL);
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
    updateDescriptorSet(renderDescriptor, renderImageView, sampler,
                        uniformBuffer);
}

void updateDescriptorSet(ShaderDescriptorSet* descriptor, ImageView* texture,
                         TextureSampler* sampler, Buffer* uniformBuffer) {
    VkDescriptorBufferInfo* bufferInfo = createBufferInfo(uniformBuffer);

    VkDescriptorImageInfo* imageInfo = createImageInfo(texture, sampler);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, bufferInfo,
                             NULL);

    descriptor->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 1,
                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, NULL,
                             imageInfo);

    delete imageInfo;
    delete bufferInfo;
}

RenderModule::~RenderModule() {}