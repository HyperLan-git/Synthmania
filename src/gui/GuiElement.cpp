#include "GuiElement.hpp"

GuiElement::GuiElement(ImageView* texture, std::string name)
    : Gui(texture, name) {}

bool GuiElement::isInside(glm::vec2 position) {
    return position.x <= (this->position.x + this->size.x / 2) &&
           position.x >= (this->position.x - this->size.x / 2) &&
           position.y <= (this->position.y + this->size.y / 2) &&
           position.y >= (this->position.y - this->size.y / 2);
}

GuiElement::~GuiElement() {}