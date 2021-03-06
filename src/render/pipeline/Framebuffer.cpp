#include "Framebuffer.hpp"

Framebuffer::Framebuffer(Device *device, RenderPass *pass, VkExtent2D extent,
                         std::vector<ImageView *> views) {
    this->device = device;
    this->framebuffer = new VkFramebuffer();
    VkImageView v[views.size()];
    for (int i = 0; i < views.size(); i++) v[i] = *(views[i]->getView());
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = *(pass->getPass());
    framebufferInfo.attachmentCount = views.size();
    framebufferInfo.pAttachments = v;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(*(device->getDevice()), &framebufferInfo, nullptr,
                            framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

VkFramebuffer *Framebuffer::getFramebuffer() { return framebuffer; }

Framebuffer::~Framebuffer() {
    vkDestroyFramebuffer(*(device->getDevice()), *framebuffer, nullptr);
    delete framebuffer;
}