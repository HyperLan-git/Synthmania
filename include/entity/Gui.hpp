#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Entity.hpp"
#include "Image.hpp"
#include "Model.hpp"

struct GuiData {
    alignas(16) glm::vec3 pos;
    alignas(4) float rot;
    alignas(8) glm::vec2 size;
};

class Gui {
   public:
    Gui(ImageView* texture, const char* name);

    ImageView* getTexture() const;
    const char* getName() const;
    glm::vec3 getPosition() const;
    float getRotation() const;
    glm::vec2 getSize() const;

    void setPosition(glm::vec2 pos);
    void setZ(float z);
    void setSize(glm::vec2 size);

    void setDestroyed();
    bool isDestroyed();

    virtual bool update(float time);

    virtual ShaderData* getShaderData() const;

    ~Gui();

   protected:
    ImageView* texture;
    glm::vec3 position = {0, 0, 0};
    float rotation = 0;
    glm::vec2 size = {1, 1};
    const char* name;
    bool destroyed = false;
};