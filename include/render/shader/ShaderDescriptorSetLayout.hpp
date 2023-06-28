#pragma once

class ShaderDescriptorSetLayout;

#include <stdexcept>

#include "Device.hpp"

/**
 * @brief A descriptor is a way to give access to program variables for shaders.
 * It is used for uniforms and texture images among other things.
 * Each binding corresponds to a single global variable and describes how and
 * when it might be accessed by the shader code.
 */
class ShaderDescriptorSetLayout {
   public:
    ShaderDescriptorSetLayout(Device* device,
                              VkDescriptorSetLayoutBinding* bindings,
                              uint32_t nBindings);
    VkDescriptorSetLayout* getLayout();
    ~ShaderDescriptorSetLayout();

   private:
    Device* device;
    VkDescriptorSetLayout* layout;
};