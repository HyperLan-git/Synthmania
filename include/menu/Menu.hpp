#pragma once

class Menu;

#include <vector>

#include "Button.hpp"
#include "Game.hpp"

// TODO config file menus
// FIXME whoops I broke graphics test

class Menu {
   public:
    Menu(Game *g);

    std::vector<Button *> getButtons();

    std::vector<GuiElement *> getGuiElements();

    std::vector<Gui *> getGuis();

    /**
     * @brief Use this function to add everything this menu needs to the game
     * instance. Don't forget to reset everything in case your menu gets loaded
     * a second time.
     */
    virtual void show();

    virtual void onPressed(Button *b);

    virtual void update(int64_t time);

    virtual ~Menu();

   protected:
    std::vector<Gui *> guis;
    std::vector<GuiElement *> elements;
    std::vector<Button *> buttons;
    Game *game;
};