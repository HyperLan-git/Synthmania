#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>
#include <vector>

typedef struct VkVertexInputAttributeDescription VIADesc;

std::array<VIADesc, 2> getAttributeDescriptions();

VkVertexInputBindingDescription getBindingDescription();

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};

class Model
{
public:
    Model(const std::vector<struct Vertex> vdata);

private:
    std::vector<struct Vertex> vdata;
};
