#include "Framebuffer.hpp"

Framebuffer::Framebuffer(RenderPass& pass, VkExtent2D extent,
                         std::vector<TexPtr> views)
    : device(pass.getDevice()), pass(pass) {
    this->extent = extent;
    VkImageView v[views.size()];
    for (int i = 0; i < views.size(); i++) v[i] = views[i]->getView();
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = pass.getPass();
    framebufferInfo.attachmentCount = views.size();
    framebufferInfo.pAttachments = v;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr,
                            &framebuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create framebuffer!");
}

VkFramebuffer Framebuffer::getFramebuffer() { return framebuffer; }

VkExtent2D Framebuffer::getExtent() { return extent; }

Device& Framebuffer::getDevice() { return device; }

RenderPass& Framebuffer::getRenderPass() { return pass; }

Framebuffer::~Framebuffer() {
    vkDestroyFramebuffer(device.getDevice(), framebuffer, nullptr);
}