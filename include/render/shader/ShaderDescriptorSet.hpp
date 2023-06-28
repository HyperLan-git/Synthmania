#pragma once

class ShaderDescriptorSet;

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Buffer.hpp"
#include "ShaderDescriptorPool.hpp"
#include "ShaderDescriptorSetLayout.hpp"

/**
 * @brief The descriptor set is the immutable list of descriptors associated
 * with a single swapchain. It needs to be instantiated before uniforms and
 * texture samplers can be used.
 */
class ShaderDescriptorSet {
   public:
    ShaderDescriptorSet(Device* device, ShaderDescriptorPool* pool,
                        ShaderDescriptorSetLayout* layout);
    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorBufferInfo* bInfo,
                      VkDescriptorImageInfo* iInfo);

    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorBufferInfo* bInfo);

    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorImageInfo* iInfo);

    void resetAccess();

    VkDescriptorSet* getSet();

    ~ShaderDescriptorSet();

   private:
    Device* device;
    VkDescriptorSet* set;
    ShaderDescriptorPool* pool;
    VkWriteDescriptorSet* writeDescriptor = NULL;
};