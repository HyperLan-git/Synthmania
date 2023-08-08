#pragma once

class ShaderDescriptorSetLayout;

#include "Device.hpp"

/**
 * @brief A descriptor is a way to give access to program variables for shaders.
 * It is used for uniforms and texture images among other things.
 * Each binding corresponds to a single global variable and describes how and
 * when it might be accessed by the shader code.
 */
class ShaderDescriptorSetLayout {
   public:
    ShaderDescriptorSetLayout(
        Device& device,
        std::initializer_list<VkDescriptorSetLayoutBinding> bindings);

    ShaderDescriptorSetLayout(ShaderDescriptorSetLayout&&) = delete;
    ShaderDescriptorSetLayout& operator=(ShaderDescriptorSetLayout&&) = delete;

    ShaderDescriptorSetLayout(const ShaderDescriptorSetLayout&) = delete;
    ShaderDescriptorSetLayout& operator=(const ShaderDescriptorSetLayout&) =
        delete;

    VkDescriptorSetLayout getLayout();
    Device& getDevice();

    ~ShaderDescriptorSetLayout();

   private:
    Device& device;
    VkDescriptorSetLayout layout;
};