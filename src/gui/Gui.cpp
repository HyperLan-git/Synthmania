#include "Gui.hpp"

#ifndef M_PI
#define M_PI 3.14159265359
#endif

Gui::Gui(Texture texture, std::string name) {
    this->texture = texture;
    this->name = name;
}

Texture Gui::getTexture() const { return texture; }
std::string Gui::getName() const { return name; }
glm::vec3 Gui::getPosition() const { return position; }
glm::vec2 Gui::getRealPosition() const { return position; }
glm::vec2 Gui::getGraphicalPosition() const { return graphicalPosition; }
float Gui::getRotation() const {
    return (rotation - (int)(rotation / (M_PI * 2)) * 2 * M_PI);
}
glm::vec2 Gui::getSize() const { return size; }
glm::vec4 Gui::getColor() const { return color; }
int Gui::getNegate() const { return negate; }

void Gui::setPosition(glm::vec2 pos) {
    this->position = glm::vec3(pos.x, pos.y, position.z);
}

void Gui::setZ(float z) { this->position.z = z; }

void Gui::setSize(glm::vec2 size) { this->size = size; }

void Gui::setColor(glm::vec4 color) { this->color = color; }

void Gui::setNegate(int negate) { this->negate = negate; }

void Gui::setTexture(Texture texture) { this->texture = texture; }

bool Gui::update(int64_t time) { return false; }

void Gui::updateGraphics(int64_t time) {
    this->graphicalPosition = {0, 0};
    for (GraphicalEffect* e : this->effects)
        this->graphicalPosition += (*e)(this, time);
}

bool Gui::isDestroyed() { return this->destroyed; }

void Gui::setDestroyed() { this->destroyed = true; }

void Gui::addEffect(GraphicalEffect* effect) {
    this->effects.push_back(effect);
}

std::vector<GraphicalEffect*>& Gui::getGraphicalEffects() { return effects; }

ShaderData Gui::getShaderData() const {
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    glm::vec3 p = glm::vec3(graphicalPosition, 0);
    p += position;
    edata->pos = p;
    edata->rot = rotation;
    edata->size = size;
    edata->color = color;
    if (negate) edata->color.a *= -1;
    return {edata, sizeof(GuiData)};
}

Gui::~Gui() {
    for (GraphicalEffect* e : this->effects) delete e;
}