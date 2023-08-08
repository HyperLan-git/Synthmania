#pragma once

class ComputeModule;

#include "ShaderDescriptorSet.hpp"

class ComputeModule : public boost::noncopyable {
   public:
    ComputeModule(CommandPool& pool, ComputeShader& shader,
                  std::initializer_list<VkPushConstantRange> pushConstants,
                  std::initializer_list<VkDescriptorSetLayoutBinding> bindings,
                  VkDeviceSize* bufferSizes, uint32_t nBuffers);

    void run(Queue* queue, void* constants, VkDeviceSize sz, uint64_t workers);

    void fillBuffer(uint32_t buffer, void* data);
    void emptyBuffer(uint32_t buffer, void* data);

    ~ComputeModule();

   private:
    Device& device;
    ShaderDescriptorPool* shaderPool;
    ShaderDescriptorSetLayout* shaderLayout;
    ShaderDescriptorSet* shaderSet;
    PipelineLayout* layout;
    Pipeline* pipeline;
    CommandPool& pool;
    CommandBuffer* commandBuffer;
    std::vector<Buffer*> buffers;
    ComputeShader& shader;
};