#pragma once

class RenderModule;

#include "CommandBuffer.hpp"
#include "Framebuffer.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
#include "Renderer.hpp"

class RenderModule {
   public:
    RenderModule(Instance* instance, VkPhysicalDevice* physicalDevice,
                 Device* device, std::string name, uint32_t w, uint32_t h,
                 RenderPass* renderPass, VkDescriptorSetLayoutBinding* bindings,
                 uint32_t nBindings,
                 VkPipelineShaderStageCreateInfo* shaderStages,
                 uint32_t nShaders);
    RenderModule(Renderer* r, std::string name);

    void recreate();
    void recreateImage(int w, int h);

    ~RenderModule();

   private:
    Image* renderImage = NULL;
    ImageView* renderImageView = NULL;
    Image* depthImage = NULL;
    ImageView* depthImageView = NULL;
    Framebuffer* framebuffer = NULL;
    CommandBuffer* renderCommandBuffer = NULL;
    Pipeline* renderPipeline = NULL;
    PipelineLayout* renderPipelineLayout = NULL;
    ShaderDescriptorSetLayout* renderLayout = NULL;
    ShaderDescriptorSet* renderDescriptor = NULL;
    ShaderDescriptorPool* renderDescriptorPool = NULL;
    Buffer* uniformBuffer = NULL;
    TextureSampler* sampler = NULL;
    CommandPool* commandPool = NULL;
};

void updateDescriptorSet(ShaderDescriptorSet* descriptor, ImageView* texture,
                         TextureSampler* sampler, Buffer* uniformBuffer);