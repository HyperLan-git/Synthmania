#include "Checkbox.hpp"

Checkbox::Checkbox(Texture unchecked, Texture checked, std::string name,
                   bool isChecked)
    : MenuElement(unchecked, name) {
    this->checkedTex = checked;
    this->uncheckedTex = unchecked;
    this->check(isChecked);
}

void Checkbox::onClicked(glm::vec2 pos) { check(!checked); }

bool Checkbox::isChecked() { return checked; }

void Checkbox::check(bool value) {
    this->checked = value;
    this->texture = value ? checkedTex : uncheckedTex;
}