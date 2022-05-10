#include "Gui.hpp"

Gui::Gui(ImageView* texture, const char* name) {
    this->texture = texture;
    this->name = name;
}

ImageView* Gui::getTexture() const { return texture; }
const char* Gui::getName() const { return name; }
glm::vec3 Gui::getPosition() const { return position; }
float Gui::getRotation() const { return (rotation - ((int)rotation / (M_PI))); }
glm::vec2 Gui::getSize() const { return size; }
glm::vec4 Gui::getColor() const { return color; }

void Gui::setPosition(glm::vec2 pos) {
    this->position = glm::vec3(pos.x, pos.y, position.z);
}

void Gui::setZ(float z) { this->position.z = z; }

void Gui::setSize(glm::vec2 size) { this->size = size; }

bool Gui::update(float time) { return false; }

bool Gui::isDestroyed() { return this->destroyed; }

void Gui::setDestroyed() { this->destroyed = true; }

ShaderData* Gui::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    edata->pos = position;
    edata->rot = rotation;
    edata->size = size;
    edata->color = color;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

Gui::~Gui() {}