#pragma once

class PipelineLayout;

#include "ShaderDescriptorSetLayout.hpp"

class PipelineLayout {
   public:
    PipelineLayout(ShaderDescriptorSetLayout& shaderLayout,
                   std::initializer_list<VkPushConstantRange> constantRanges);

    PipelineLayout(PipelineLayout&&) = delete;
    PipelineLayout& operator=(PipelineLayout&&) = delete;

    VkPipelineLayout getLayout();

    Device& getDevice();

    ~PipelineLayout();

   private:
    Device& device;
    VkPipelineLayout layout;
};