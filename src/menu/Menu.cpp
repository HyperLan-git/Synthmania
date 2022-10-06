#include "Menu.hpp"

Menu::Menu(Game* g) {
    this->game = g;
    this->selected = NULL;
}

std::vector<Button*> Menu::getButtons() { return buttons; }

std::vector<MenuElement*> Menu::getMenuElements() { return elements; }

std::vector<Gui*> Menu::getGuis() { return guis; }

void Menu::select(MenuElement* element) {
    if (selected != NULL) selected->focus(false);
    if (element != NULL) element->focus();
    this->selected = element;
}

void Menu::show() {
    for (Button* b : buttons) game->addGui(b);
    for (MenuElement* e : elements) game->addGui(e);
    for (Gui* g : guis) game->addGui(g);
    game->getWindow()->setTextcallback([](GLFWwindow* window, unsigned int c) {
        Game* g = (Game*)(glfwGetWindowUserPointer(window));
        if (g == NULL) return;
        Menu* menu = dynamic_cast<Menu*>(g->getCurrentMenu());
        if (menu == NULL) return;
        menu->textCallback(window, c);
    });
    game->getWindow()->setKeycallback(
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Game* g = (Game*)(glfwGetWindowUserPointer(window));
            if (g == NULL) return;
            Menu* menu = g->getCurrentMenu();
            if (menu == NULL) return;
            menu->keyCallback(window, key, scancode, action, mods);
        });
}

void Menu::onPressed(Button* b) {}

void Menu::update(int64_t time) {}

void Menu::textCallback(GLFWwindow* window, unsigned int codepoint) {
    if (selected != NULL) selected->onTyped(codepoint);
}

void Menu::keyCallback(GLFWwindow* window, int key, int scancode, int action,
                       int mods) {
    if (action & GLFW_PRESS && selected != NULL)
        selected->onKeyPressed(key, scancode, mods);
}

Menu::~Menu() {
    for (MenuElement* e : elements) delete e;
    for (Button* b : buttons) delete b;
    for (Gui* g : guis) delete g;
}