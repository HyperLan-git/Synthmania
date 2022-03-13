#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>

typedef struct VkVertexInputAttributeDescription VIADesc;

std::array<VIADesc, 3> getAttributeDescriptions();

VkVertexInputBindingDescription getBindingDescription();

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

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
    Model(const std::vector<struct Vertex> vdata);

   private:
    std::vector<struct Vertex> vdata;
};
