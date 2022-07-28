#include "Menu.hpp"

Menu::Menu(Game* g) { this->game = g; }

std::vector<Button*> Menu::getButtons() { return buttons; }

std::vector<Gui*> Menu::getGuis() { return guis; };

void Menu::show() {
    for (Button* b : buttons) game->addGui(b);
    for (Gui* g : guis) game->addGui(g);
}

void Menu::onPressed(Button* b) {}

void Menu::update(int64_t time) {}

Menu::~Menu() {
    for (Button* b : buttons) delete b;
    for (Gui* g : guis) delete g;
}