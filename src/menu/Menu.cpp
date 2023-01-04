#include "Menu.hpp"

Menu::Menu(Game* g) {
    this->game = g;
    this->selected = NULL;
}

const std::vector<std::shared_ptr<Button>>& Menu::getButtons() {
    return buttons;
}

const std::vector<std::shared_ptr<MenuElement>>& Menu::getMenuElements() {
    return elements;
}

const std::vector<std::shared_ptr<Gui>>& Menu::getGuis() { return guis; }

void Menu::select(const std::shared_ptr<MenuElement>& element) {
    if (selected) selected->focus(false);
    if (element) element->focus();
    this->selected = element;
}

void Menu::unselect() {
    if (selected) selected->focus(false);
    this->selected = std::shared_ptr<MenuElement>();
}

void Menu::show() {
    for (std::shared_ptr<Button>& b : buttons) game->addTGui(b);
    for (std::shared_ptr<MenuElement>& e : elements) game->addTGui(e);
    for (std::shared_ptr<Gui>& g : guis) game->addGui(g);
    game->getWindow()->setTextcallback([](GLFWwindow* window, unsigned int c) {
        Game* g = (Game*)(glfwGetWindowUserPointer(window));
        if (!g) return;
        Menu* menu = dynamic_cast<Menu*>(g->getCurrentMenu());
        if (!menu) return;
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

void Menu::onPressed(const std::shared_ptr<Button>& b) {}

void Menu::update(int64_t time) {}

void Menu::textCallback(GLFWwindow* window, unsigned int codepoint) {
    if (selected != NULL) selected->onTyped(codepoint);
}

void Menu::keyCallback(GLFWwindow* window, int key, int scancode, int action,
                       int mods) {
    if (action & GLFW_PRESS && selected != NULL)
        selected->onKeyPressed(key, scancode, mods);
}

Menu::~Menu() {}