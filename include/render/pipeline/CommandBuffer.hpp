#pragma once

class CommandBuffer;

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Fence.hpp"
#include "Framebuffer.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "Semaphore.hpp"
#include "ShaderDescriptorSet.hpp"
#include "Swapchain.hpp"

class CommandBuffer {
   public:
    CommandBuffer(Device *device, CommandPool *commandPool, bool singleTime);

    void begin();
    void reset();
    void end();

    VkCommandBuffer *getBuffer();

    void setImageLayout(Image *image, VkImageLayout oldLayout,
                        VkImageLayout newLayout);

    void copyImage(Image *src, VkImageLayout srcImageLayout, Image *dst,
                   VkImageLayout dstImageLayout);

    void convertImage(Image *src, VkImageLayout srcImageLayout, Image *dst,
                      VkImageLayout dstImageLayout, VkFilter filter);

    void copyBufferToImage(Buffer *srcBuffer, Image *image,
                           VkExtent3D imageExtent);

    void copyBufferRegion(Buffer *src, Buffer *dest, VkDeviceSize size);

    void beginRenderPass(Swapchain *swapchain, uint32_t imageIndex,
                         VkClearValue *clearValues, uint32_t count);
    void endRenderPass();
    void bindPipeline(Pipeline *pipeline);
    void bindVertexBuffers(Buffer *vertexBuffers, uint32_t count);
    void bindIndexBuffer(Buffer *indexBuffer);
    void bindDescriptorSet(Pipeline *pipeline,
                           const ShaderDescriptorSet *descriptorSet);
    void pushConstants(Pipeline *pipeline, VkShaderStageFlags shaderStage,
                       uint32_t offset, const void *data, uint32_t size);
    void draw(uint32_t count);

    void submit(Queue *queue, Semaphore *waitSemaphore,
                VkPipelineStageFlags waitStage, Semaphore *finishedSemaphore,
                Fence *fence);
    void submit(Queue *queue, bool wait = true);

    ~CommandBuffer();

   private:
    VkCommandBuffer *buffer;
    Device *device;
    CommandPool *pool;
    bool singleTime;
};