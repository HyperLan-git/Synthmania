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

Model loadFromFile(Device& device, const char* obj);

class Model {
   public:
    Model(Device& device, const std::vector<Vertex>& vdata,
          const std::vector<uint16_t>& idata);

    Model(Model&&);
    Model& operator=(Model&&);

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Buffer& toVertexBuffer();
    Buffer& toIndicesBuffer();

    std::vector<Vertex> getVertexes();
    std::vector<uint16_t> getIndexes();

    ~Model();

   private:
    Device& device;
    std::vector<Vertex> vdata;
    std::vector<uint16_t> idata;
    std::unique_ptr<Buffer> vertexBuffer, indexBuffer;
    void createVertexBuffer(Device& device);
    void createIndexBuffer(Device& device);
};
