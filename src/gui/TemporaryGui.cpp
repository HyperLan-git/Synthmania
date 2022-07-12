#include "TemporaryGui.hpp"

#include "Gui.hpp"

TemporaryGui::TemporaryGui(ImageView* texture, const char* name, int64_t fade,
                           int64_t death)
    : Gui(texture, name) {
    this->fade = fade;
    this->death = death;
}

void TemporaryGui::setFinalPosition(glm::vec2 pos) {
    this->finalPosition = pos;
}

void TemporaryGui::setPosition(glm::vec2 pos) {
    Gui::setPosition(pos);
    initialPosition = pos;
    glm::vec2 t = pos;
    t.y -= .2f;
    setFinalPosition(t);
}

bool TemporaryGui::update(int64_t time) {
    double progress =
        (long double)(time - this->fade) / (this->death - this->fade);
    this->position.x =
        progress * finalPosition.x + (1 - progress) * initialPosition.x;
    this->position.y =
        progress * finalPosition.y + (1 - progress) * initialPosition.y;
    this->color.a = 1 - progress;
    return time >= this->death;
}