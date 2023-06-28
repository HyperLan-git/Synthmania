#pragma once

struct Vertex;
class Model;

#include <glm/glm.hpp>

#include "Buffer.hpp"
#include "Device.hpp"
#include "tiny_obj_loader.hpp"

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

class Model {
   public:
    Model(const std::vector<Vertex> vdata, const std::vector<uint16_t> idata,
          VkPhysicalDevice* physicalDevice, Device* device);
    Model(const char* obj, VkPhysicalDevice* physicalDevice, Device* device);

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&);
    Model& operator=(Model&&);

    Buffer* toVertexBuffer();
    Buffer* toIndicesBuffer();

    std::vector<Vertex> getVertexes();
    std::vector<uint16_t> getIndexes();

    ~Model();

   private:
    std::vector<Vertex> vdata;
    std::vector<uint16_t> idata;
    Buffer *vertexBuffer, *indexBuffer;
    void createVertexBuffer(VkPhysicalDevice* physicalDevice, Device* device);
    void createIndexBuffer(VkPhysicalDevice* physicalDevice, Device* device);
};
