#pragma once

class RenderModule;

#include "CommandBuffer.hpp"
#include "Framebuffer.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
#include "Renderer.hpp"
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
                 uint32_t nBindings,
                 VkPipelineShaderStageCreateInfo* shaderStages,
                 uint32_t nShaders, VkPushConstantRange* constantRanges,
                 uint32_t nConstantRange);

    void setExtent(uint32_t w, uint32_t h);
    void setShaders(VkPipelineShaderStageCreateInfo* shaderStages,
                    uint32_t nShaders);

    void recreate();

    ~RenderModule();

    // private:
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
    ShaderDescriptorSet* renderDescriptor = NULL;
    ShaderDescriptorPool* renderDescriptorPool = NULL;
    Buffer* uniformBuffer = NULL;
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
};

void updateDescriptorSet(ShaderDescriptorSet* descriptor, ImageView* texture,
                         TextureSampler* sampler, int binding);

void updateDescriptorSet(ShaderDescriptorSet* descriptor, Buffer* uniformBuffer,
                         int binding);