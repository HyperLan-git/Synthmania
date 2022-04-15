#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Entity.hpp"
#include "Image.hpp"
#include "Model.hpp"

struct GuiData {
    alignas(8) glm::vec2 pos;
    alignas(4) float rot;
    alignas(8) glm::vec2 size;
};

class Gui {
   public:
    Gui(Image* texture, const char* name);

    Image* getTexture() const;
    const char* getName() const;
    glm::vec2 getPosition() const;
    float getRotation() const;
    glm::vec2 getSize() const;

    virtual bool update(float time);

    virtual ShaderData* getShaderData() const;

    ~Gui();

   protected:
    Image* texture;
    glm::vec2 position = {0, 0};
    float rotation = 0;
    glm::vec2 size = {1, 1};  // Entire screen
    const char* name;
};