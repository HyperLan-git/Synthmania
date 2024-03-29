#include "MenuElement.hpp"

MenuElement::MenuElement(Texture texture, std::string name)
    : Gui(texture, name) {}

void MenuElement::focus(bool focus) { this->selected = focus; }

bool MenuElement::isInside(glm::vec2 position) {
    return position.x <= (this->position.x + this->size.x / 2) &&
           position.x >= (this->position.x - this->size.x / 2) &&
           position.y <= (this->position.y + this->size.y / 2) &&
           position.y >= (this->position.y - this->size.y / 2);
}

void MenuElement::onPressed(glm::vec2 pos) {}

void MenuElement::onClicked(glm::vec2 pos) {}

void MenuElement::onKeyPressed(int key, int scancode, int mods) {}

void MenuElement::onTyped(unsigned int character) {}

MenuElement::~MenuElement() {}