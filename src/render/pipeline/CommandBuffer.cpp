#include "CommandBuffer.hpp"

CommandBuffer::CommandBuffer(Device *device, CommandPool *commandPool,
                             bool singleTime) {
    this->device = device;
    this->pool = commandPool;
    this->buffer = new VkCommandBuffer();
    this->singleTime = singleTime;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = *(commandPool->getPool());
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(*(device->getDevice()), &allocInfo, buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (singleTime)
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(*buffer, &beginInfo);
}

VkCommandBuffer *CommandBuffer::getBuffer() { return buffer; }

void CommandBuffer::end() {
    if (vkEndCommandBuffer(*buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandBuffer::reset() {
    vkResetCommandBuffer(*buffer,
                         /*VkCommandBufferResetFlagBits*/ 0);
}

void CommandBuffer::submit(Queue *queue) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffer;

    vkQueueSubmit(*(queue->getQueue()), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(*(queue->getQueue()));
}

void CommandBuffer::submit(Queue *queue, Semaphore *waitSemaphore,
                           Semaphore *finishedSemaphore, Fence *fence) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphore->getSemaphore();
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffer;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = finishedSemaphore->getSemaphore();

    if (vkQueueSubmit(*(queue->getQueue()), 1, &submitInfo,
                      *(fence->getFence())) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void CommandBuffer::beginRenderPass(Swapchain *swapchain, uint32_t imageIndex,
                                    VkClearValue *clearValues, uint32_t count) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = *(swapchain->getRenderPass()->getPass());
    renderPassInfo.framebuffer =
        *(swapchain->getFramebuffers()[imageIndex]->getFramebuffer());
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain->getExtent();

    renderPassInfo.clearValueCount = count;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(*buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bindPipeline(Pipeline *pipeline) {
    vkCmdBindPipeline(*buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      *(pipeline->getPipeline()));
}

void CommandBuffer::bindVertexBuffers(Buffer *vertexBuffers, uint32_t count) {
    VkDeviceSize offsets[count] = {0};
    VkBuffer vertex[count];
    for (uint32_t i = 0; i < count; i++)
        vertex[i] = *(vertexBuffers[i].getBuffer());
    vkCmdBindVertexBuffers(*buffer, 0, count, vertex, offsets);
}

void CommandBuffer::bindIndexBuffer(Buffer *indexBuffer) {
    vkCmdBindIndexBuffer(*buffer, *(indexBuffer->getBuffer()), 0,
                         VK_INDEX_TYPE_UINT16);
}

void CommandBuffer::bindDescriptorSet(
    Pipeline *pipeline, const ShaderDescriptorSet *descriptorSet) {
    vkCmdBindDescriptorSets(*buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            *(pipeline->getLayout()->getLayout()), 0, 1,
                            descriptorSet->getSet(), 0, nullptr);
}

void CommandBuffer::draw(uint32_t count) {
    vkCmdDrawIndexed(*buffer, count, 1, 0, 0, 0);
}

void CommandBuffer::pushConstants(Pipeline *pipeline,
                                  VkShaderStageFlags shaderStage,
                                  uint32_t offset, const void *data,
                                  uint32_t size) {
    vkCmdPushConstants(*buffer, *(pipeline->getLayout()->getLayout()),
                       shaderStage, offset, size, data);
}

void CommandBuffer::endRenderPass() { vkCmdEndRenderPass(*buffer); }

void CommandBuffer::setImageLayout(Image *image, VkImageLayout oldLayout,
                                   VkImageLayout newLayout) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = *(image->getImage());
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    switch (oldLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier.srcAccessMask = 0;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        default:
            throw std::invalid_argument("unsupported layout transition!");
    }
    switch (newLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier.dstAccessMask = 0;
            destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            destinationStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                               VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        default:
            throw std::invalid_argument("unsupported layout transition!");
    }
    vkCmdPipelineBarrier(*buffer, sourceStage, destinationStage, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);
}

void CommandBuffer::copyBufferToImage(Buffer *srcBuffer, Image *image,
                                      VkExtent3D imageExtent) {
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = imageExtent;

    vkCmdCopyBufferToImage(*buffer, *(srcBuffer->getBuffer()),
                           *(image->getImage()),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void CommandBuffer::copyBufferRegion(Buffer *src, Buffer *dest,
                                     VkDeviceSize size) {
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(*buffer, *(src->getBuffer()), *(dest->getBuffer()), 1,
                    &copyRegion);
}

void CommandBuffer::copyImage(Image *src, VkImageLayout srcImageLayout,
                              Image *dst, VkImageLayout dstImageLayout) {
    VkImageCopy regions;
    regions.srcOffset = regions.dstOffset = {0, 0, 0};
    regions.extent = src->getExtent();
    regions.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    regions.srcSubresource.mipLevel = 0;
    regions.srcSubresource.baseArrayLayer = 0;
    regions.srcSubresource.layerCount = 1;
    regions.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    regions.dstSubresource.mipLevel = 0;
    regions.dstSubresource.baseArrayLayer = 0;
    regions.dstSubresource.layerCount = 1;
    vkCmdCopyImage(*buffer, *(src->getImage()), srcImageLayout,
                   *(dst->getImage()), dstImageLayout, 1, &regions);
}

CommandBuffer::~CommandBuffer() {
    vkFreeCommandBuffers(*(device->getDevice()), *(pool->getPool()), 1, buffer);
    delete buffer;
}