#pragma once

struct Vertex;
struct UniformBufferObject;
class Model;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <tiny_obj_loader.h>

#include <array>
#include <cstring>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

#include "Buffer.hpp"
#include "Device.hpp"

typedef struct VkVertexInputAttributeDescription VIADesc;

std::array<VIADesc, 2> getAttributeDescriptions();

VkVertexInputBindingDescription getBindingDescription();

struct Vertex {
    glm::vec3 pos;
    glm::vec2 texCoord;
    bool operator==(const Vertex& other) const;
};
template <>
struct std::hash<Vertex> {
    size_t operator()(Vertex const& vertex) const;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class Model {
   public:
    Model(const std::vector<Vertex> vdata, const std::vector<uint16_t> idata,
          VkPhysicalDevice* physicalDevice, Device* device);
    Model(const char* obj, VkPhysicalDevice* physicalDevice, Device* device);

    Buffer* toVertexBuffer();
    Buffer* toIndicesBuffer();

    std::vector<struct Vertex> getVertexes();
    std::vector<uint16_t> getIndexes();

    ~Model();

   private:
    std::vector<struct Vertex> vdata;
    std::vector<uint16_t> idata;
    Buffer *vertexBuffer, *indexBuffer;
    void createVertexBuffer(VkPhysicalDevice* physicalDevice, Device* device);
    void createIndexBuffer(VkPhysicalDevice* physicalDevice, Device* device);
};
