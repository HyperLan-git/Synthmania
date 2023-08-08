#pragma once

class CommandBuffer;

#include "ComputeShader.hpp"
#include "Fence.hpp"
#include "Framebuffer.hpp"
#include "Pipeline.hpp"
#include "Semaphore.hpp"
#include "ShaderDescriptorSet.hpp"

class CommandBuffer {
   public:
    CommandBuffer(CommandPool &commandPool, bool singleTime,
                  bool secondary = false);

    CommandBuffer(CommandBuffer &&);
    CommandBuffer &operator=(CommandBuffer &&);

    CommandBuffer(const CommandBuffer &);
    CommandBuffer &operator=(const CommandBuffer &);

    VkResult begin();
    VkResult reset(VkCommandBufferResetFlags flags = 0);
    VkResult end();

    VkCommandBuffer getBuffer();

    void setViewport(float width, float height);

    void setScissor(VkExtent2D extent);

    void setImageLayout(Image &image, VkImageLayout oldLayout,
                        VkImageLayout newLayout, uint32_t layer = 0,
                        uint32_t numLayers = 1);

    void copyImage(Image &src, VkImageLayout srcImageLayout, Image &dst,
                   VkImageLayout dstImageLayout,
                   VkOffset3D srcOffset = {0, 0, 0},
                   VkOffset3D dstOffset = {0, 0, 0},
                   VkExtent3D extent = {0, 0, 0}, uint32_t layer = 0);

    void convertImage(Image &src, VkImageLayout srcImageLayout, Image &dst,
                      VkImageLayout dstImageLayout, VkFilter filter);

    void copyBufferToImage(Buffer &srcBuffer, Image &image,
                           VkExtent3D imageExtent);

    void copyBufferRegion(Buffer &src, Buffer &dest, VkDeviceSize size);

    void beginRenderPass(Framebuffer &framebuffer, RenderPass &renderPass,
                         std::initializer_list<VkClearValue> clearValues);
    void endRenderPass();
    void bindPipeline(Pipeline &pipeline, VkPipelineBindPoint bindPoint =
                                              VK_PIPELINE_BIND_POINT_GRAPHICS);
    void bindVertexBuffers(std::initializer_list<Buffer *> vertexBuffers);
    void bindIndexBuffer(Buffer &indexBuffer);
    void bindDescriptorSet(
        Pipeline &pipeline, ShaderDescriptorSet &descriptorSet,
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);
    void pushConstants(Pipeline &pipeline, VkShaderStageFlags shaderStage,
                       uint32_t offset, const void *data, uint32_t size);
    void draw(uint32_t count);

    VkResult submit(const Queue &queue, Semaphore &waitSemaphore,
                    VkPipelineStageFlags waitStage,
                    Semaphore &finishedSemaphore, Fence &fence);
    VkResult submit(const Queue &queue, bool wait = true);

    void executeComputeShader(ComputeShader &shader, uint64_t workers,
                              uint64_t workGroups = 1,
                              uint64_t workLegions = 1);

    void executeCommandBuffer(CommandBuffer &secondary);

    ~CommandBuffer();

   private:
    Device &device;
    VkCommandBuffer buffer;
    CommandPool &pool;
    bool singleTime, secondary;
};