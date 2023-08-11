#pragma once

class Pipeline;

#include "ImageView.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"

class Pipeline {
   public:
    Pipeline(
        PipelineLayout &layout, RenderPass &renderPass,
        VkExtent2D swapChainExtent,
        std::initializer_list<VkPipelineShaderStageCreateInfo> shaderStages);
    Pipeline(PipelineLayout &layout, RenderPass &renderPass,
             VkExtent2D swapChainExtent,
             const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages);

    Pipeline(PipelineLayout &layout, ComputeShader &shader);

    Pipeline(Pipeline &&) = delete;
    Pipeline &operator=(Pipeline &&) = delete;
    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    VkPipeline getPipeline();
    PipelineLayout &getLayout();

    Device &getDevice();

    bool isGraphics();

    ~Pipeline();

   private:
    Device &device;
    VkPipeline pipeline;
    PipelineLayout &layout;
    bool graphics;
};