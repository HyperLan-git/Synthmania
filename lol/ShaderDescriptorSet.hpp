#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Buffer.hpp"
#include "ShaderDescriptorPool.hpp"
#include "ShaderDescriptorSetLayout.hpp"

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

/**
 * @brief The descriptor set is the immutable list of descriptors associated
 * with a single swapchain. It needs to be instantiated before uniforms and
 * texture samplers can be used.
 *
 */
VkDescriptorBufferInfo* createBufferInfo(Buffer* buffer);
VkDescriptorImageInfo* createImageInfo(VkImageView view, VkSampler sampler);
class ShaderDescriptorSet {
   public:
    ShaderDescriptorSet(VkDevice* device, ShaderDescriptorPool* pool,
                        ShaderDescriptorSetLayout* layout);
    void updateAccess(VkStructureType allowed, uint32_t binding,
                      VkDescriptorType type, VkDescriptorBufferInfo* bInfo,
                      VkDescriptorImageInfo* iInfo);

    VkDescriptorSet* getSet();
    ~ShaderDescriptorSet();

   private:
    VkDevice* device;
    VkDescriptorSet* set;
    ShaderDescriptorPool* pool;
    VkWriteDescriptorSet* writeDescriptor = nullptr;
};