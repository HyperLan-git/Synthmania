#pragma once

class ShaderDescriptorSet;

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Buffer.hpp"
#include "ImageView.hpp"
#include "Model.hpp"
#include "ShaderDescriptorPool.hpp"
#include "ShaderDescriptorSetLayout.hpp"
#include "TextureSampler.hpp"

VkDescriptorBufferInfo* createBufferInfo(Buffer* buffer);
VkDescriptorImageInfo* createImageInfo(ImageView* view,
                                       TextureSampler* sampler);

/**
 * @brief The descriptor set is the immutable list of descriptors associated
 * with a single swapchain. It needs to be instantiated before uniforms and
 * texture samplers can be used.
 */
class ShaderDescriptorSet {
   public:
    ShaderDescriptorSet(VkDevice* device, ShaderDescriptorPool* pool,
                        ShaderDescriptorSetLayout* layout);
    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorBufferInfo* bInfo,
                      VkDescriptorImageInfo* iInfo);

    VkDescriptorSet* getSet() const;
    ~ShaderDescriptorSet();

   private:
    VkDevice* device;
    VkDescriptorSet* set;
    ShaderDescriptorPool* pool;
    VkWriteDescriptorSet* writeDescriptor = nullptr;
};