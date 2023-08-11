#include "ComputeModule.hpp"

ComputeModule::ComputeModule(
    CommandPool& pool, ComputeShader& shader,
    std::initializer_list<VkPushConstantRange> pushConstants,
    std::initializer_list<VkDescriptorSetLayoutBinding> bindings,
    std::initializer_list<VkDeviceSize> buffers)
    : device(pool.getDevice()), pool(pool), shader(shader) {
    std::initializer_list<VkDescriptorPoolSize> sizes;
    std::vector<VkDescriptorPoolSize> poolSz(bindings.size());
    auto iter1 = bindings.begin();
    for (int i = 0; i < bindings.size(); i++, iter1++)
        poolSz[i] =
            VkDescriptorPoolSize{.type = iter1->descriptorType,
                                 .descriptorCount = iter1->descriptorCount};
    shaderPool = std::make_unique<ShaderDescriptorPool>(device, poolSz);
    shaderLayout = new ShaderDescriptorSetLayout(device, bindings);
    shaderSet = new ShaderDescriptorSet(*shaderPool, *shaderLayout);
    int i = 0;
    for (auto it = buffers.begin(); it != buffers.end(); it++, i++) {
        VkDescriptorBufferInfo info = {
            .buffer = this->buffers
                          .emplace_back(
                              device, *it, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                          .getBuffer(),
            .offset = 0,
            .range = *it};
        shaderSet->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, i,
                                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &info, NULL);
    }
    layout = new PipelineLayout(*shaderLayout, pushConstants);
    pipeline = new Pipeline(*layout, shader);
    commandBuffer = new CommandBuffer(pool, true);
}

void ComputeModule::run(Queue& queue, void* constants, VkDeviceSize sz,
                        uint64_t workers) {
    commandBuffer->reset();

    commandBuffer->begin();
    commandBuffer->bindPipeline(*pipeline, VK_PIPELINE_BIND_POINT_COMPUTE);
    commandBuffer->bindDescriptorSet(*pipeline, *shaderSet,
                                     VK_PIPELINE_BIND_POINT_COMPUTE);
    commandBuffer->pushConstants(*pipeline, VK_SHADER_STAGE_COMPUTE_BIT, 0,
                                 constants, sz);

    // TODO handle multidimension
    commandBuffer->executeComputeShader(shader, workers);

    commandBuffer->end();
    // TODO check if is done handle async
    commandBuffer->submit(queue);
}

void ComputeModule::fillBuffer(uint32_t buffer, void* data) {
    buffers[buffer].fill(data);
}

void ComputeModule::emptyBuffer(uint32_t buffer, void* data) {
    buffers[buffer].empty(data);
}

ComputeModule::~ComputeModule() {
    device.wait();
    buffers.clear();
    delete commandBuffer;
    delete pipeline;
    delete layout;
    delete shaderSet;
    delete shaderLayout;
    shaderPool.reset();
}