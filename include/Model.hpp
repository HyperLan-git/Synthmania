#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <cstring>
#include <glm/glm.hpp>
#include <vector>

#include "Buffer.hpp"

typedef struct VkVertexInputAttributeDescription VIADesc;

std::array<VIADesc, 3> getAttributeDescriptions();

VkVertexInputBindingDescription getBindingDescription();

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class Model {
   public:
    Model(const std::vector<Vertex> vdata, const std::vector<uint16_t> idata);

    Buffer* toVertexBuffer(VkPhysicalDevice* physicalDevice, VkDevice* device);
    Buffer* toIndicesBuffer(VkPhysicalDevice* physicalDevice, VkDevice* device);

   private:
    std::vector<struct Vertex> vdata;
    std::vector<uint16_t> idata;
};
