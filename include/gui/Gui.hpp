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
    alignas(16) glm::vec4 color;
};

// TODO make a beatiful artwork of programming where multiple classe's logic can
// be merged into one ie. you can have a shaking note or a temporary and
// parented gui
class Gui {
   public:
    Gui(ImageView* texture, const char* name);

    ImageView* getTexture() const;
    const char* getName() const;
    glm::vec3 getPosition() const;
    glm::vec2 getGraphicalPosition() const;
    float getRotation() const;
    glm::vec2 getSize() const;
    glm::vec4 getColor() const;

    void setPosition(glm::vec2 pos);
    void setGraphicalPosition(glm::vec2 pos);
    void setZ(float z);
    void setSize(glm::vec2 size);
    void setColor(glm::vec4 color);

    void setDestroyed();
    bool isDestroyed();

    // Updates and returns true if gui SHOULD DIE :skull:
    virtual bool update(int64_t time);

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
    bool destroyed = false;
};