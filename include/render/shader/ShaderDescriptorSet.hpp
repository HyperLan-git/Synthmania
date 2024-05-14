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
    ShaderDescriptorSet(ShaderDescriptorPool& pool,
                        ShaderDescriptorSetLayout& layout);

    ShaderDescriptorSet(ShaderDescriptorSet&&) = delete;
    ShaderDescriptorSet& operator=(ShaderDescriptorSet&&) = delete;

    ShaderDescriptorSet(const ShaderDescriptorSet&) = delete;
    ShaderDescriptorSet& operator=(const ShaderDescriptorSet&) = delete;

    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorBufferInfo* bInfo,
                      VkDescriptorImageInfo* iInfo);

    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorBufferInfo* bInfo);

    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorImageInfo* iInfo);

    void resetAccess();

    VkDescriptorSet getSet();
    Device& getDevice();

    ~ShaderDescriptorSet();

   private:
    Device& device;
    VkDescriptorSet set;
    ShaderDescriptorPool& pool;
    // XXX a bit of shitty pointer magic to fix
    VkWriteDescriptorSet* writeDescriptor;
};