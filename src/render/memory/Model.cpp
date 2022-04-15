#define TINYOBJLOADER_IMPLEMENTATION
#include "Model.hpp"

typedef struct VkVertexInputAttributeDescription VIADesc;

VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VIADesc, 2> getAttributeDescriptions() {
    std::array<VIADesc, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, Vertex::pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, Vertex::texCoord);

    return attributeDescriptions;
}

bool Vertex::operator==(const Vertex& other) const {
    return pos == other.pos && texCoord == other.texCoord;
}

// When You
size_t std::hash<Vertex>::operator()(Vertex const& vertex) const {
    return (((int)(vertex.pos.x * 182480327) ^ (int)(vertex.pos.y * 94574341) ^
             (int)(vertex.pos.z * 58295969)) >>
            1) ^
           (((int)(vertex.texCoord.x * 53185423) ^
             (int)(vertex.texCoord.y * 37247279))
            << 1);
}

Model::Model(const std::vector<Vertex> vdata, const std::vector<uint16_t> idata,
             VkPhysicalDevice* physicalDevice, Device* device) {
    this->vdata = std::vector<Vertex>(vdata.size());
    this->idata = std::vector<uint16_t>(idata.size());
    for (int i = 0; i < vdata.size(); i++) this->vdata[i] = vdata[i];

    for (int i = 0; i < idata.size(); i++) this->idata[i] = idata[i];
    createVertexBuffer(physicalDevice, device);
    createIndexBuffer(physicalDevice, device);
}

Model::Model(const char* obj, VkPhysicalDevice* physicalDevice,
             Device* device) {
    this->vdata = std::vector<Vertex>();
    this->idata = std::vector<uint16_t>();
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj)) {
        throw std::runtime_error(warn + err);
    }
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                          attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2]};

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vdata.size());
                vdata.push_back(vertex);
            }

            idata.push_back(uniqueVertices[vertex]);
        }
    }
    createVertexBuffer(physicalDevice, device);
    createIndexBuffer(physicalDevice, device);
}

Buffer* Model::toVertexBuffer() { return vertexBuffer; }

Buffer* Model::toIndicesBuffer() { return indexBuffer; }

void Model::createVertexBuffer(VkPhysicalDevice* physicalDevice,
                               Device* device) {
    VkDeviceSize bufferSize = sizeof(vdata[0]) * vdata.size();

    vertexBuffer = new Buffer(physicalDevice, device, bufferSize,
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(*(device->getDevice()),
                *(vertexBuffer->getMemory()->getMemory()), 0, bufferSize, 0,
                &data);
    memcpy(data, vdata.data(), (size_t)bufferSize);
    vkUnmapMemory(*(device->getDevice()),
                  *(vertexBuffer->getMemory()->getMemory()));
}

void Model::createIndexBuffer(VkPhysicalDevice* physicalDevice,
                              Device* device) {
    VkDeviceSize bufferSize = sizeof(idata[0]) * idata.size();

    indexBuffer = new Buffer(physicalDevice, device, bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(*(device->getDevice()),
                *(indexBuffer->getMemory()->getMemory()), 0, bufferSize, 0,
                &data);
    memcpy(data, idata.data(), (size_t)bufferSize);
    vkUnmapMemory(*(device->getDevice()),
                  *(indexBuffer->getMemory()->getMemory()));
}

std::vector<struct Vertex> Model::getVertexes() { return vdata; }

std::vector<uint16_t> Model::getIndexes() { return idata; }

Model::~Model() {
    delete vertexBuffer;
    delete indexBuffer;
}