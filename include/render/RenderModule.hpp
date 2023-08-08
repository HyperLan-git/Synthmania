#pragma once

class RenderModule;

#include "Framebuffer.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
#include "TextureSampler.hpp"

/**
 * @brief A simplified class for creating a pipeline that can be used for
 * parallel/multi-pass rendering. All setters will wait for the device to be
 * available and recreate the inner pipeline !
 */
class RenderModule : public boost::noncopyable {
   public:
    RenderModule(
        Instance& instance, Device& device, std::string name, uint32_t w,
        uint32_t h, RenderPass& renderPass,
        std::initializer_list<VkDescriptorSetLayoutBinding> descriptorBindings,
        std::initializer_list<VkPipelineShaderStageCreateInfo> shaderStages,
        std::initializer_list<VkPushConstantRange> constantRanges,
        uint32_t* nDescriptorSets);

    Pipeline* getPipeline();

    ShaderDescriptorSet* getDescriptorSet(ImageView* texture, size_t current);

    ShaderDescriptorSet* addDescriptorSet(ImageView* img, Buffer* buffer);

    void setExtent(uint32_t w, uint32_t h);
    void setShaders(
        std::initializer_list<VkPipelineShaderStageCreateInfo> shaderStages);
    void setDescriptorLayouts(VkDescriptorSetLayoutBinding* bindings,
                              uint32_t* nDescriptorSets, uint32_t nBindings);

    void recreateDescriptorPool(VkDescriptorType* types,
                                uint32_t* nDescriptorSets, uint32_t nTypes);

    void recreate();

    ~RenderModule();

   private:
    Instance& instance;
    Device& device;

    ImageView* renderImageView = NULL;
    // TODO make depth optional
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
    RenderPass& renderPass;

    // For debugging xdd
    std::string name;

    std::initializer_list<VkDescriptorSetLayoutBinding> descriptorBindings;
    std::initializer_list<VkPipelineShaderStageCreateInfo> shaderStages;
    std::initializer_list<VkPushConstantRange> constantRanges;

    VkExtent2D extent;
};

void updateDescriptorSet(ShaderDescriptorSet& descriptor, ImageView& texture,
                         TextureSampler& sampler, Buffer& uniformBuffer);
