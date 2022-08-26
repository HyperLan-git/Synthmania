#pragma once

class ComputeModule;

#include <string>
#include <vector>

#include "Buffer.hpp"
#include "CommandPool.hpp"
#include "ComputeShader.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "Shader.hpp"
#include "ShaderDescriptorPool.hpp"
#include "ShaderDescriptorSet.hpp"
#include "ShaderDescriptorSetLayout.hpp"
#include "Utils.hpp"

class ComputeModule {
   public:
    ComputeModule(VkPhysicalDevice* physicalDevice, Device* device,
                  CommandPool* pool, ComputeShader* shader,
                  VkPushConstantRange* pushConstants, uint32_t nConstants,
                  VkDescriptorSetLayoutBinding* bindings, uint32_t nBindings,
                  VkDeviceSize* bufferSizes, uint32_t nBuffers);

    void run(Queue* queue, void* constants, VkDeviceSize sz, uint64_t workers);

    void fillBuffer(uint32_t buffer, void* data);
    void emptyBuffer(uint32_t buffer, void* data);

    ~ComputeModule();

   private:
    Device* device;
    ShaderDescriptorPool* shaderPool;
    ShaderDescriptorSetLayout* shaderLayout;
    ShaderDescriptorSet* shaderSet;
    PipelineLayout* layout;
    Pipeline* pipeline;
    CommandPool* pool;
    CommandBuffer* commandBuffer;
    std::vector<Buffer*> buffers;
    ComputeShader* shader;
};