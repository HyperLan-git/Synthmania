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

void Gui::setPosition(glm::vec3 pos) { this->position = pos; }

bool Gui::update(float time) { return false; }

ShaderData* Gui::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    edata->pos = position;
    edata->rot = rotation;
    edata->size = size;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

Gui::~Gui() {}