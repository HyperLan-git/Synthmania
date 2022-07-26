#include "Button.hpp"

Button::Button(ImageView* texture, ImageView* pressedTexture, const char* name,
               glm::vec2 position, glm::vec2 size)
    : Gui(texture, name) {
    this->normalTexture = texture;
    this->pressedTexture = pressedTexture;
    this->position = glm::vec3(position, 0);
    this->size = size;
}

bool Button::isInside(glm::vec2 position) {
    return position.x <= (this->position.x + this->size.x / 2) &&
           position.x >= (this->position.x - this->size.x / 2) &&
           position.y <= (this->position.y + this->size.y / 2) &&
           position.y >= (this->position.y - this->size.y / 2);
}

void Button::onPressed() { this->texture = pressedTexture; }

void Button::onReleased() { this->texture = normalTexture; }

Button::~Button() {}