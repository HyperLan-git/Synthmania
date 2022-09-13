#include "Button.hpp"

Button::Button(ImageView* texture, ImageView* pressedTexture, std::string name,
               glm::vec2 position, glm::vec2 size)
    : MenuElement(texture, name) {
    this->normalTexture = texture;
    this->pressedTexture = pressedTexture;
    this->position = glm::vec3(position, 0);
    this->size = size;
}

bool Button::isInside(glm::vec2 position) {
    return MenuElement::isInside(position);
}

bool Button::isPressed() { return pressed; }

void Button::onPressed(glm::vec2 position) {
    if (pressed) return;
    this->texture = pressedTexture;
    pressed = true;
}

void Button::onReleased() {
    if (!pressed) return;
    this->texture = normalTexture;
    pressed = false;
}

Button::~Button() {}