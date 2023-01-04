#pragma once

class Menu;

#include <vector>

#include "Button.hpp"
#include "Game.hpp"

// TODO config file menus
// TODO navigate with keyboard or piano keys (how???)
// FIXME whoops I broke graphics test

class Menu {
   public:
    Menu(Game *g);

    const std::vector<std::shared_ptr<Button>> &getButtons();
    const std::vector<std::shared_ptr<MenuElement>> &getMenuElements();
    const std::vector<std::shared_ptr<Gui>> &getGuis();

    void unselect();
    void select(const std::shared_ptr<MenuElement> &element);

    /**
     * @brief Use this function to add everything this menu needs to the game
     * instance. Don't forget to reset everything in case your menu gets loaded
     * a second time.
     */
    virtual void show();

    virtual void onPressed(const std::shared_ptr<Button> &b);

    virtual void update(int64_t time);

    void textCallback(GLFWwindow *window, unsigned int codepoint);
    void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods);

    virtual ~Menu();

   protected:
    std::vector<std::shared_ptr<Gui>> guis;
    std::vector<std::shared_ptr<MenuElement>> elements;
    std::vector<std::shared_ptr<Button>> buttons;
    std::shared_ptr<MenuElement> selected;
    Game *game;
};