#include "Model.hpp"
typedef struct VkVertexInputAttributeDescription VIADesc;

VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VIADesc, 3> getAttributeDescriptions() {
    std::array<VIADesc, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}

Model::Model(const std::vector<Vertex> vdata,
             const std::vector<uint16_t> idata) {
    this->vdata = std::vector<Vertex>(vdata.size());
    this->idata = std::vector<uint16_t>(idata.size());
    for (int i = 0; i < vdata.size(); i++) this->vdata[i] = vdata[i];

    for (int i = 0; i < idata.size(); i++) this->idata[i] = idata[i];
}

Buffer* Model::toVertexBuffer(VkPhysicalDevice* physicalDevice,
                              VkDevice* device) {
    VkDeviceSize bufferSize = sizeof(vdata[0]) * vdata.size();

    Buffer* vertexBuffer = new Buffer(physicalDevice, device, bufferSize,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(*device, *(vertexBuffer->getMemory()->getMemory()), 0,
                bufferSize, 0, &data);
    memcpy(data, vdata.data(), (size_t)bufferSize);
    vkUnmapMemory(*device, *(vertexBuffer->getMemory()->getMemory()));

    return vertexBuffer;
}

Buffer* Model::toIndicesBuffer(VkPhysicalDevice* physicalDevice,
                               VkDevice* device) {
    VkDeviceSize bufferSize = sizeof(idata[0]) * idata.size();

    Buffer* indicesBuffer = new Buffer(
        physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(*device, *(indicesBuffer->getMemory()->getMemory()), 0,
                bufferSize, 0, &data);
    memcpy(data, idata.data(), (size_t)bufferSize);
    vkUnmapMemory(*device, *(indicesBuffer->getMemory()->getMemory()));
    return indicesBuffer;
}