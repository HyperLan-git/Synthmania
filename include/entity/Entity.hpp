#pragma once

struct ShaderData;
struct EntityData;

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ImageView.hpp"
#include "Model.hpp"

struct ShaderData {
    void* data;
    uint32_t size;
};

struct EntityData {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec4 rot;
    alignas(16) glm::vec3 size;
};

class Entity {
   public:
    Entity(Model* model, ImageView* texture, const char* name);

    Model* getModel() const;
    ImageView* getTexture() const;
    const char* getName() const;
    glm::vec3 getPosition() const;
    glm::vec4 getRotation() const;
    glm::vec3 getSize() const;

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const;

    ~Entity();

   protected:
    Model* model;
    ImageView* texture;
    glm::vec3 position = {0, 0, 0};
    glm::vec4 rotation = {0, 0, 0, 1};  // Quaternions my beloved :heart:
    glm::vec3 size = {1, 1, 1};
    const char* name;
};