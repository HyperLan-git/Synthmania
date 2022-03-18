#pragma once

class Pipeline;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Model.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"

class Pipeline {
   public:
    Pipeline(VkDevice *device, PipelineLayout *layout, RenderPass *renderPass,
             VkPipelineShaderStageCreateInfo *shaderStages,
             uint32_t shaderCount, VkExtent2D swapChainExtent);
    VkPipeline *getPipeline();
    PipelineLayout *getLayout();
    ~Pipeline();

   private:
    VkDevice *device;
    VkPipeline *pipeline;
    PipelineLayout *layout;
};