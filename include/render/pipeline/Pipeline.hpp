#pragma once

class Pipeline;

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Model.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"

class Pipeline {
   public:
    Pipeline(Device *device, PipelineLayout *layout, RenderPass *renderPass,
             VkPipelineShaderStageCreateInfo *shaderStages,
             uint32_t shaderCount, VkExtent2D swapChainExtent);
    VkPipeline *getPipeline();
    PipelineLayout *getLayout();
    ~Pipeline();

   private:
    Device *device;
    VkPipeline *pipeline;
    PipelineLayout *layout;
};