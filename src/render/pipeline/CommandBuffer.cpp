#include "CommandBuffer.hpp"

CommandBuffer::CommandBuffer(CommandPool &commandPool, bool singleTime,
                             bool secondary)
    : device(commandPool.getDevice()), pool(commandPool) {
    this->singleTime = singleTime;
    this->secondary = secondary;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.getPool();
    allocInfo.level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY
                                : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &buffer) !=
        VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");
}

CommandBuffer::CommandBuffer(CommandBuffer &&other)
    : device(other.device), pool(other.pool) {
    *this = std::move(other);
}

CommandBuffer &CommandBuffer::operator=(CommandBuffer &&other) {
    assert(this->device == other.device);
    std::swap(this->buffer, other.buffer);
    return *this;
}

CommandBuffer::CommandBuffer(const CommandBuffer &other)
    : CommandBuffer(other.pool, other.singleTime, other.secondary) {}

CommandBuffer &CommandBuffer::operator=(const CommandBuffer &other) {
    assert(this->device == other.device);
    this->secondary = other.secondary;
    this->singleTime = other.singleTime;
    if (buffer)
        vkFreeCommandBuffers(device.getDevice(), pool.getPool(), 1, &buffer);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = this->pool.getPool();
    allocInfo.level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY
                                : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &buffer) !=
        VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");
    return *this;
}

VkResult CommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (singleTime)
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    return vkBeginCommandBuffer(buffer, &beginInfo);
}

VkCommandBuffer CommandBuffer::getBuffer() { return buffer; }

void CommandBuffer::setViewport(float width, float height) {
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buffer, 0, 1, &viewport);
}

void CommandBuffer::setScissor(VkExtent2D extent) {
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(buffer, 0, 1, &scissor);
}

VkResult CommandBuffer::end() { return vkEndCommandBuffer(buffer); }

VkResult CommandBuffer::reset(VkCommandBufferResetFlags flags) {
    return vkResetCommandBuffer(buffer, flags);
}

VkResult CommandBuffer::submit(const Queue &queue, bool wait) {
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pNext = NULL;

    VkResult res =
        vkQueueSubmit(queue.getQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    if (res) return res;
    if (wait) return queue.wait();
    return VK_SUCCESS;
}

VkResult CommandBuffer::submit(const Queue &queue, Semaphore &waitSemaphore,
                               VkPipelineStageFlags waitStage,
                               Semaphore &finishedSemaphore, Fence &fence) {
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;

    VkSemaphore sem1 = waitSemaphore.getSemaphore(),
                sem2 = finishedSemaphore.getSemaphore();

    VkPipelineStageFlags waitStages[] = {waitStage};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &sem1;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &sem2;

    return vkQueueSubmit(queue.getQueue(), 1, &submitInfo, fence.getFence());
}

void CommandBuffer::beginRenderPass(
    Framebuffer &framebuffer, std::initializer_list<VkClearValue> clearValues) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = framebuffer.getRenderPass().getPass();
    renderPassInfo.framebuffer = framebuffer.getFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = framebuffer.getExtent();

    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.begin();

    vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bindPipeline(Pipeline &pipeline,
                                 VkPipelineBindPoint bindPoint) {
    vkCmdBindPipeline(buffer, bindPoint, pipeline.getPipeline());
}

void CommandBuffer::bindVertexBuffers(
    std::initializer_list<Buffer *> vertexBuffers) {
    VkDeviceSize offsets[vertexBuffers.size()] = {0};
    VkBuffer vertex[vertexBuffers.size()];
    auto iter = vertexBuffers.begin();
    for (uint32_t i = 0; i != vertexBuffers.size(); i++, iter++)
        vertex[i] = (*iter)->getBuffer();
    vkCmdBindVertexBuffers(buffer, 0, vertexBuffers.size(), vertex, offsets);
}

void CommandBuffer::bindIndexBuffer(Buffer &indexBuffer) {
    vkCmdBindIndexBuffer(buffer, indexBuffer.getBuffer(), 0,
                         VK_INDEX_TYPE_UINT16);
}

void CommandBuffer::bindDescriptorSet(Pipeline &pipeline,
                                      ShaderDescriptorSet &descriptorSet,
                                      VkPipelineBindPoint bindPoint) {
    VkDescriptorSet s = descriptorSet.getSet();
    vkCmdBindDescriptorSets(buffer, bindPoint, pipeline.getLayout().getLayout(),
                            0, 1, &s, 0, NULL);
}

void CommandBuffer::draw(uint32_t count) {
    vkCmdDrawIndexed(buffer, count, 1, 0, 0, 0);
}

void CommandBuffer::pushConstants(Pipeline &pipeline,
                                  VkShaderStageFlags shaderStage,
                                  uint32_t offset, const void *data,
                                  uint32_t size) {
    vkCmdPushConstants(buffer, pipeline.getLayout().getLayout(), shaderStage,
                       offset, size, data);
}

void CommandBuffer::endRenderPass() { vkCmdEndRenderPass(buffer); }

void CommandBuffer::setImageLayout(Image &image, VkImageLayout oldLayout,
                                   VkImageLayout newLayout, uint32_t layer,
                                   uint32_t numLayers) {
    VkImageMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.getImage();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = layer;
    barrier.subresourceRange.layerCount = numLayers;
    barrier.pNext = NULL;

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
    vkCmdPipelineBarrier(buffer, sourceStage, destinationStage, 0, 0, NULL, 0,
                         NULL, 1, &barrier);
}

void CommandBuffer::copyBufferToImage(Buffer &srcBuffer, Image &image,
                                      VkExtent3D imageExtent) {
    VkBufferImageCopy region;
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = imageExtent;

    vkCmdCopyBufferToImage(buffer, srcBuffer.getBuffer(), image.getImage(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void CommandBuffer::copyBufferRegion(Buffer &src, Buffer &dest,
                                     VkDeviceSize size) {
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(buffer, src.getBuffer(), dest.getBuffer(), 1, &copyRegion);
}

void CommandBuffer::copyImage(Image &src, VkImageLayout srcImageLayout,
                              Image &dst, VkImageLayout dstImageLayout,
                              VkOffset3D srcOffset, VkOffset3D dstOffset,
                              VkExtent3D extent, uint32_t layer) {
    VkImageCopy regions;
    regions.srcOffset = srcOffset;
    regions.dstOffset = dstOffset;
    bool nullExtent =
        extent.width == 0 && extent.height == 0 && extent.depth == 0;
    regions.extent = nullExtent ? src.getExtent() : extent;
    regions.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    regions.srcSubresource.mipLevel = 0;
    regions.srcSubresource.baseArrayLayer = 0;
    regions.srcSubresource.layerCount = 1;
    regions.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    regions.dstSubresource.mipLevel = 0;
    regions.dstSubresource.baseArrayLayer = layer;
    regions.dstSubresource.layerCount = 1;
    vkCmdCopyImage(buffer, src.getImage(), srcImageLayout, dst.getImage(),
                   dstImageLayout, 1, &regions);
}

void CommandBuffer::convertImage(Image &src, VkImageLayout srcImageLayout,
                                 Image &dst, VkImageLayout dstImageLayout,
                                 VkFilter filter) {
    VkImageBlit blit;
    blit.srcOffsets[0] = blit.dstOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = blit.dstOffsets[1] = {1, 1, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = 0;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = 0;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;
    vkCmdBlitImage(buffer, src.getImage(), srcImageLayout, dst.getImage(),
                   dstImageLayout, 1, &blit, filter);
}

void CommandBuffer::executeComputeShader(ComputeShader &shader,
                                         uint64_t workers, uint64_t workGroups,
                                         uint64_t workLegions) {
    vkCmdDispatch(buffer, workers, workGroups, workLegions);
}

void CommandBuffer::executeCommandBuffer(CommandBuffer &secondary) {
    if (!secondary.secondary)
        throw std::invalid_argument("Command buffer must be secondary !");
    VkCommandBuffer cmd = secondary.getBuffer();
    vkCmdExecuteCommands(buffer, 1, &cmd);
}

CommandBuffer::~CommandBuffer() {
    if (buffer)
        vkFreeCommandBuffers(device.getDevice(), pool.getPool(), 1, &buffer);
}