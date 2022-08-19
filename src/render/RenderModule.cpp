#include "RenderModule.hpp"

RenderModule::RenderModule(Renderer* r, std::string name, int w, int h) {
    DebugFunc functions = getDebugFunctions(r->getInstance());
    renderImage = new Image(
        r->getPhysicalDevice(), r->getDevice(), w, h, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    renderImageView =
        new ImageView(r->getDevice(), renderImage, VK_FORMAT_R8G8B8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT, "render");
    sampler = new TextureSampler(r->getPhysicalDevice(), r->getDevice());
    setName(functions, r->getDevice(), "render image sampler",
            VK_OBJECT_TYPE_SAMPLER, *(sampler->getSampler()));
}

RenderModule::~RenderModule() {}