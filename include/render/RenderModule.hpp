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
    RenderModule(Renderer* r, std::string name, int w, int h);
    RenderModule(Renderer* r, std::string name);

    void recreate();
    void recreateImage(int w, int h);

    ~RenderModule();

   private:
    Renderer* r;
    Image* renderImage = NULL;
    ImageView* renderImageView = NULL;
    Image* depthImage = NULL;
    ImageView* depthImageView = NULL;
    RenderPass* renderPass = NULL;
    CommandBuffer* renderCommandBuffer = NULL;
    Pipeline* renderPipeline = NULL;
    PipelineLayout* renderPipelineLayout = NULL;
    ShaderDescriptorSetLayout* renderLayout = NULL;
    ShaderDescriptorSet* renderDescriptor = NULL;
    Semaphore* imageAvailableSemaphore = NULL;
    Buffer* uniformBuffer = NULL;
    TextureSampler* sampler = NULL;
};