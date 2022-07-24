#pragma once

class Gui;

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

#include "Entity.hpp"
#include "GraphicalEffect.hpp"
#include "Image.hpp"
#include "Model.hpp"

struct GuiData {
    alignas(16) glm::vec3 pos;
    alignas(4) float rot;
    alignas(8) glm::vec2 size;
    alignas(16) glm::vec4 color;
};

class Gui {
   public:
    Gui(ImageView* texture, const char* name);

    ImageView* getTexture() const;
    const char* getName() const;
    glm::vec3 getPosition() const;
    glm::vec2 getGraphicalPosition() const;
    virtual glm::vec2 getRealPosition() const;
    float getRotation() const;
    glm::vec2 getSize() const;
    glm::vec4 getColor() const;

    void setPosition(glm::vec2 pos);
    void setZ(float z);
    void setSize(glm::vec2 size);
    void setColor(glm::vec4 color);

    void addEffect(GraphicalEffect* effect);

    std::vector<GraphicalEffect*>& getGraphicalEffects();

    void setDestroyed();
    bool isDestroyed();

    // Updates and returns true if gui SHOULD DIE :skull:
    virtual bool update(int64_t time);
    void updateGraphics(int64_t time);

    // Data for the shader, will depend on the shader
    virtual ShaderData* getShaderData() const;

    virtual ~Gui() = default;

   protected:
    ImageView* texture;
    glm::vec3 position = {0, 0, 0};
    glm::vec2 graphicalPosition = {0, 0};
    float rotation = 0;
    glm::vec2 size = {1, 1};
    glm::vec4 color = {1, 1, 1, 1};
    const char* name;
    std::vector<GraphicalEffect*> effects;
    bool destroyed = false;
};