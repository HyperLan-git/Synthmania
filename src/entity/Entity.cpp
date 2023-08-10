#include "Entity.hpp"

#include <math.h>
#include <stdlib.h>

Entity::Entity(Model& model, Texture texture, std::string name) : model(model) {
    this->texture = texture;
    this->name = name;
}

Model& Entity::getModel() const { return this->model; }
Texture Entity::getTexture() const { return this->texture; }

glm::vec3 Entity::getPosition() const { return this->position; }
glm::vec4 Entity::getRotation() const { return this->rotation; }
glm::vec3 Entity::getSize() const { return this->size; }

std::string Entity::getName() const { return this->name; }

ShaderData Entity::getShaderData() const {
    EntityData* edata = (EntityData*)malloc(sizeof(EntityData));
    edata->pos = position;
    edata->rot = rotation;
    edata->size = size;
    return {edata, sizeof(EntityData)};
}

bool Entity::update(int64_t time) {
    double t = time / 1000000.;
    this->rotation.z = sin(t);
    this->rotation.w = cos(t);
    this->rotation = glm::normalize(this->rotation);
    return false;
}

Entity::~Entity() {}
