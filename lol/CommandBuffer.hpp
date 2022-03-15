#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Buffer.hpp"
#include "CommandPool.hpp"
#include "Fence.hpp"
#include "Framebuffer.hpp"
#include "Image.hpp"
#include "Semaphore.hpp"
#include "ShaderDescriptorSet.hpp"

class CommandBuffer {
   public:
    CommandBuffer(VkDevice *device, CommandPool *commandPool, bool singleTime);

    void begin();
    void reset();
    void end();

    VkCommandBuffer *getBuffer();

    void setImageLayout(Image *image, VkImageLayout oldLayout,
                        VkImageLayout newLayout);

    void copyBufferToImage(Buffer *srcBuffer, Image *image,
                           VkExtent3D imageExtent);

    void copyBufferRegion(Buffer *src, Buffer *dest, VkDeviceSize size);

    void beginRenderPass(VkRenderPass renderPass, Framebuffer *framebuffer,
                         VkExtent2D extent, VkClearValue *clearValues,
                         uint32_t count);
    void endRenderPass();
    void bindPipeline(VkPipeline pipeline);
    void bindVertexBuffers(Buffer *vertexBuffers, uint32_t count);
    void bindIndexBuffer(Buffer *indexBuffer);
    void bindDescriptorSet(VkPipelineLayout pipelineLayout,
                           const ShaderDescriptorSet *descriptorSet);
    void draw(uint32_t count);

    void submit(VkQueue queue, Semaphore *waitSemaphore,
                Semaphore *finishedSemaphore, Fence *fence);
    void submit(VkQueue queue);

    ~CommandBuffer();

   private:
    VkCommandBuffer *buffer;
    VkDevice *device;
    CommandPool *pool;
    bool singleTime;
};