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

    std::vector<Button *> getButtons();
    std::vector<MenuElement *> getMenuElements();
    std::vector<Gui *> getGuis();

    void select(MenuElement *element);

    /**
     * @brief Use this function to add everything this menu needs to the game
     * instance. Don't forget to reset everything in case your menu gets loaded
     * a second time.
     */
    virtual void show();

    virtual void onPressed(Button *b);

    virtual void update(int64_t time);

    void textCallback(GLFWwindow *window, unsigned int codepoint);
    void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods);

    virtual ~Menu();

   protected:
    std::vector<Gui *> guis;
    std::vector<MenuElement *> elements;
    std::vector<Button *> buttons;
    MenuElement *selected;
    Game *game;
};