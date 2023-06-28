#pragma once

class Gui;

#include "Entity.hpp"
#include "GraphicalEffect.hpp"
#include "Image.hpp"
#include "Model.hpp"

#define ALIGN(type) alignas(sizeof(type)) type
#define ALIGN_VEC(n, type) alignas(sizeof(type) * n) glm::vec<n, type>

struct GuiData {
    alignas(sizeof(float) * 4) glm::vec3 pos;
    ALIGN(float) rot;
    alignas(sizeof(float) * 2) glm::vec2 size;
    alignas(sizeof(float) * 4) glm::vec4 color;
};

class Gui {
   public:
    Gui(ImageView* texture, std::string name);

    ImageView* getTexture() const;
    std::string getName() const;
    glm::vec3 getPosition() const;
    glm::vec2 getGraphicalPosition() const;
    virtual glm::vec2 getRealPosition() const;
    float getRotation() const;
    glm::vec2 getSize() const;
    glm::vec4 getColor() const;
    int getNegate() const;

    void setPosition(glm::vec2 pos);
    void setZ(float z);
    void setSize(glm::vec2 size);
    void setColor(glm::vec4 color);
    void setNegate(int negate);
    void setTexture(ImageView* texture);

    void addEffect(GraphicalEffect* effect);

    std::vector<GraphicalEffect*>& getGraphicalEffects();

    void setDestroyed();
    bool isDestroyed();

    // Updates and returns true if gui SHOULD DIE :skull:
    virtual bool update(int64_t time);
    void updateGraphics(int64_t time);

    // Data for the shader, will depend on the shader
    virtual ShaderData* getShaderData() const;

    virtual ~Gui();

   protected:
    ImageView* texture;
    glm::vec3 position = {0, 0, 0};
    glm::vec2 graphicalPosition = {0, 0};
    float rotation = 0;
    glm::vec2 size = {1, 1};
    glm::vec4 color = {1, 1, 1, 1};
    int negate = false;
    std::string name;
    std::vector<GraphicalEffect*> effects;
    bool destroyed = false;
};