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
        std::initializer_list<VkDescriptorPoolSize> sets);

    Pipeline* getPipeline();

    ShaderDescriptorSet* getDescriptorSet(TexPtr texture, size_t current);

    ShaderDescriptorSet* addDescriptorSet(TexPtr img, Buffer* buffer);

    void setExtent(uint32_t w, uint32_t h);
    void setShaders(
        std::initializer_list<VkPipelineShaderStageCreateInfo> shaderStages);
    void setDescriptorLayouts(
        std::initializer_list<VkDescriptorSetLayoutBinding> descriptorBindings);

    void recreateDescriptorPool(
        std::initializer_list<VkDescriptorPoolSize> sets);

    void recreate();

    ~RenderModule();

   private:
    Instance& instance;
    Device& device;

    TexPtr renderImageView = NULL;
    // TODO make depth optional
    TexPtr depthImageView = NULL;
    Framebuffer* framebuffer = NULL;
    Pipeline* renderPipeline = NULL;
    PipelineLayout* renderPipelineLayout = NULL;
    ShaderDescriptorSetLayout* renderLayout = NULL;
    ShaderDescriptorPool* descriptorPool = NULL;
    // Map from texture name to descriptor sets
    std::map<std::string, std::vector<ShaderDescriptorSet*>> descriptorSets;
    std::vector<Buffer*> guiUniformBuffers;
    std::vector<Buffer*> guiConstantBuffers;
    TextureSampler* sampler = NULL;
    CommandPool* commandPool = NULL;
    CommandBuffer* commandBuffer = NULL;
    RenderPass& renderPass;

    // For debugging xdd
    std::string name;

    std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    std::vector<VkPushConstantRange> constantRanges;
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

    VkExtent2D extent;
};

void updateDescriptorSet(ShaderDescriptorSet& descriptor, ImageView& texture,
                         TextureSampler& sampler, Buffer& uniformBuffer);
