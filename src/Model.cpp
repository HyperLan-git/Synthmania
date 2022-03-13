#include "Model.hpp"
typedef struct VkVertexInputAttributeDescription VIADesc;

std::array<VIADesc, 2> getAttributeDescriptions()
{
    std::array<VIADesc, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(struct Vertex, pos);
    return attributeDescriptions;
}

VkVertexInputBindingDescription getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(struct Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

Model::Model(const std::vector<struct Vertex> vdata)
{
    this->vdata = vdata;
}
