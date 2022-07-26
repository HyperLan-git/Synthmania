#include "Menu.hpp"

Menu::Menu(Game* g) { this->game = g; }

std::vector<Button*> Menu::getButtons() { return buttons; }

void Menu::show() {
    for (Button* b : buttons) game->addGui(b);
}

void Menu::onPressed(Button* b) {}

void Menu::update(int64_t time) {}

Menu::~Menu() {
    for (Button* b : buttons) delete b;
}