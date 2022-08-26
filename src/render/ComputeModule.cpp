#include "ComputeModule.hpp"

ComputeModule::ComputeModule(VkPhysicalDevice* physicalDevice, Device* device,
                             CommandPool* pool, ComputeShader* shader,
                             VkPushConstantRange* pushConstants,
                             uint32_t nConstants,
                             VkDescriptorSetLayoutBinding* bindings,
                             uint32_t nBindings, VkDeviceSize* bufferSizes,
                             uint32_t nBuffers) {
    this->pool = pool;
    this->shader = shader;
    this->device = device;
    VkDescriptorType types[nBindings];
    for (int i = 0; i < nBindings; i++) types[i] = bindings[i].descriptorType;
    shaderPool = new ShaderDescriptorPool(device, types, nBindings);
    shaderLayout = new ShaderDescriptorSetLayout(device, bindings, nBindings);
    shaderSet = new ShaderDescriptorSet(device, shaderPool, shaderLayout);
    for (int i = 0; i < nBuffers; i++) {
        buffers.push_back(new Buffer(physicalDevice, device, bufferSizes[i],
                                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        VkDescriptorBufferInfo info = {.buffer = *(buffers[i]->getBuffer()),
                                       .offset = 0,
                                       .range = bufferSizes[i]};
        shaderSet->updateAccess(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, i,
                                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &info, NULL);
    }
    layout =
        new PipelineLayout(device, shaderLayout, nConstants, pushConstants);
    pipeline = new Pipeline(device, layout, shader);
    commandBuffer = new CommandBuffer(device, pool, true);
}

void ComputeModule::run(Queue* queue, void* constants, VkDeviceSize sz,
                        uint64_t workers) {
    commandBuffer->reset();

    commandBuffer->begin();
    commandBuffer->bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_COMPUTE);
    commandBuffer->bindDescriptorSet(pipeline, shaderSet,
                                     VK_PIPELINE_BIND_POINT_COMPUTE);
    commandBuffer->pushConstants(pipeline, VK_SHADER_STAGE_COMPUTE_BIT, 0,
                                 constants, sz);

    // TODO handle multidimension
    commandBuffer->executeComputeShader(shader, workers);

    commandBuffer->end();
    // TODO check if is done handle async
    commandBuffer->submit(queue);
}

void ComputeModule::fillBuffer(uint32_t buffer, void* data) {
    VkDeviceMemory* mem = buffers[buffer]->getMemory()->getMemory();
    VkDeviceSize sz = buffers[buffer]->getSize();
    void* p;
    vkMapMemory(*(device->getDevice()), *mem, 0, sz, 0, &p);
    memcpy(p, data, sz);
    vkUnmapMemory(*(device->getDevice()), *mem);
}

void ComputeModule::emptyBuffer(uint32_t buffer, void* data) {
    VkDeviceMemory* mem = buffers[buffer]->getMemory()->getMemory();
    VkDeviceSize sz = buffers[buffer]->getSize();
    void* p;
    vkMapMemory(*(device->getDevice()), *mem, 0, sz, 0, &p);
    memcpy(data, p, sz);
    vkUnmapMemory(*(device->getDevice()), *mem);
}

ComputeModule::~ComputeModule() {
    device->wait();
    for (Buffer* b : buffers) delete b;
    delete commandBuffer;
    delete pipeline;
    delete layout;
    delete shaderSet;
    delete shaderLayout;
    delete shaderPool;
}