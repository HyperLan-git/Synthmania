#pragma once

class Pipeline;

#include "ImageView.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"

class Pipeline {
   public:
    Pipeline(Device *device, PipelineLayout *layout, RenderPass *renderPass,
             VkPipelineShaderStageCreateInfo *shaderStages,
             uint32_t shaderCount, VkExtent2D swapChainExtent);
    Pipeline(Device *device, PipelineLayout *layout, ComputeShader *shader);

    VkPipeline *getPipeline();
    PipelineLayout *getLayout();

    bool isGraphics();

    ~Pipeline();

   private:
    Device *device;
    VkPipeline *pipeline;
    PipelineLayout *layout;
    bool graphics;
};