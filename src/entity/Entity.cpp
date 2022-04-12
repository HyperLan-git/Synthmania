#include "Entity.hpp"

Entity::Entity(Model* model, Image* texture, const char* name) {
    this->model = model;
    this->texture = texture;
    this->name = name;
}

Model* Entity::getModel() const { return this->model; }
Image* Entity::getTexture() const { return this->texture; }

glm::vec3 Entity::getPosition() const { return this->position; }
glm::vec4 Entity::getRotation() const { return this->rotation; }

const char* Entity::getName() const { return this->name; }

ShaderData* Entity::getShaderData() const {
    ShaderData* data = new ShaderData();
    float* flatness = new float();
    data->data = flatness;
    data->size = sizeof(float);
    *flatness = 0;
    return data;
}

Entity::~Entity() {}
