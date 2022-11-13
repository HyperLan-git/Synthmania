#pragma once

class RenderModule;

#include "Framebuffer.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"

/**
 * @brief A simplified class for creating a pipeline that can be used for
 * parallel/multi-pass rendering. All setters will wait for the device to be
 * available and recreate the inner pipeline !
 */
class RenderModule {
   public:
    RenderModule(Instance* instance, VkPhysicalDevice* physicalDevice,
                 Device* device, std::string name, uint32_t w, uint32_t h,
                 RenderPass* renderPass, VkDescriptorSetLayoutBinding* bindings,
                 uint32_t* nDescriptorSets, uint32_t nBindings,
                 VkPipelineShaderStageCreateInfo* shaderStages,
                 uint32_t nShaders, VkPushConstantRange* constantRanges,
                 uint32_t nConstantRange);

    Pipeline* getPipeline();

    ShaderDescriptorSet* getDescriptorSet(ImageView* texture, size_t current);

    ShaderDescriptorSet* addDescriptorSet(ImageView* img, Buffer* buffer);

    void setExtent(uint32_t w, uint32_t h);
    void setShaders(VkPipelineShaderStageCreateInfo* shaderStages,
                    uint32_t nShaders);
    void setDescriptorLayouts(VkDescriptorSetLayoutBinding* bindings,
                              uint32_t* nDescriptorSets, uint32_t nBindings);

    void recreateDescriptorPool(VkDescriptorType* types,
                                uint32_t* nDescriptorSets, uint32_t nTypes);

    void recreate();

    ~RenderModule();

   private:
    Instance* instance = NULL;
    VkPhysicalDevice physicalDevice = NULL;
    Device* device = NULL;

    Image* renderImage = NULL;
    ImageView* renderImageView = NULL;
    // TODO make depth optional
    Image* depthImage = NULL;
    ImageView* depthImageView = NULL;
    Framebuffer* framebuffer = NULL;
    Pipeline* renderPipeline = NULL;
    PipelineLayout* renderPipelineLayout = NULL;
    ShaderDescriptorSetLayout* renderLayout = NULL;
    ShaderDescriptorPool* descriptorPool = NULL;
    std::map<ImageView*, std::vector<ShaderDescriptorSet*>> descriptorSets;
    std::vector<Buffer*> guiUniformBuffers;
    std::vector<Buffer*> guiConstantBuffers;
    TextureSampler* sampler = NULL;
    CommandPool* commandPool = NULL;
    CommandBuffer* commandBuffer = NULL;
    RenderPass* renderPass = NULL;

    // For debugging xdd
    std::string name;

    VkDescriptorSetLayoutBinding* descriptorBindings;
    uint32_t nDescriptorBindings;
    VkPipelineShaderStageCreateInfo* shaderStages;
    uint32_t nShaders;
    VkPushConstantRange* constantRanges;
    uint32_t nConstantRange;

    VkExtent2D extent;
};

void updateDescriptorSet(ShaderDescriptorSet* descriptor, ImageView* texture,
                         TextureSampler* sampler, Buffer* uniformBuffer);
