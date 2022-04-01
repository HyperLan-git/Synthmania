#pragma once

class Instance;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

class Instance {
   public:
    Instance(const char* name, uint32_t version, const char* engineName,
             uint32_t engineVersion, uint32_t apiVersion,
             std::vector<uint32_t> extensions,
             std::vector<uint32_t> validation_layers);

   private:
};