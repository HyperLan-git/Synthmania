#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Image.hpp"
#include "Model.hpp"

struct ShaderData {
    void* data;
    uint32_t size;
};

class Entity {
   public:
    Entity(Model* model, Image* texture, const char* name);

    Model* getModel() const;
    Image* getTexture() const;
    const char* getName() const;
    glm::vec3 getPosition() const;
    glm::vec4 getRotation() const;

    virtual ShaderData* getShaderData() const;

    ~Entity();

   protected:
    Model* model;
    Image* texture;
    glm::vec3 position;
    glm::vec4 rotation = {0, 0, 0, 1};  // Quaternions my beloved :heart:
    const char* name;
};