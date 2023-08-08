#include "Slider.hpp"

Slider::Slider(Texture texture, std::string name, float width, float min,
               float max, bool vertical)
    : MenuElement(texture, name) {
    this->min = min;
    this->max = max;
    this->vertical = vertical;
    this->pos = (min + max) / 2;
    this->width = width;
}

float Slider::getValue() { return this->pos; }

void Slider::setValue(float value) { this->pos = value; }

bool Slider::isInside(glm::vec2 position) {
    if (vertical)
        return position.x <= (this->position.x + this->size.x / 2) &&
               position.x >= (this->position.x - this->size.x / 2) &&
               position.y <=
                   (this->position.y + (this->width + this->size.y) / 2) &&
               position.y >=
                   (this->position.y - (this->width + this->size.y) / 2);

    return position.x <=
               (this->position.x + (this->width + this->size.x) / 2) &&
           position.x >=
               (this->position.x - (this->width + this->size.x) / 2) &&
           position.y <= (this->position.y + this->size.y / 2) &&
           position.y >= (this->position.y - this->size.y / 2);
}

void Slider::onPressed(glm::vec2 pos) {
    float m = this->vertical ? this->position.y - this->width / 2
                             : this->position.x - this->width / 2;
    float progress = this->vertical ? (pos.y - m) / (this->width)
                                    : (pos.x - m) / (this->width);
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
    this->pos = progress * (max - min) + min;
}

void Slider::onClicked(glm::vec2 pos) {}

ShaderData* Slider::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    glm::vec3 p = glm::vec3(graphicalPosition, 0);
    p += position;
    float progress = (pos - min) / (max - min);
    p.x += (progress - .5f) * width;
    edata->pos = p;
    edata->rot = rotation;
    edata->size = size;
    edata->color = color;
    if (negate) edata->color.a *= -1;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

Slider::~Slider() {}