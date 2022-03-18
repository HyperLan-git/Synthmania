#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
/**
 * @brief A descriptor is a way to give access to program variables for shaders.
 * It is used for uniforms and texture images among other things.
 * Each binding corresponds to a single global variable and describes how and
 * when it might be accessed by the shader code.
 */
class ShaderDescriptorSetLayout {
   public:
    ShaderDescriptorSetLayout(VkDevice* device,
                              VkDescriptorSetLayoutBinding* bindings,
                              uint32_t nBindings);
    VkDescriptorSetLayout* getLayout();
    ~ShaderDescriptorSetLayout();

   private:
    VkDevice* device;
    VkDescriptorSetLayout* layout;
};