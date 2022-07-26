#pragma once

class Menu;

#include "Button.hpp"
#include "Game.hpp"

// TODO config file menus

class Menu {
   public:
    Menu(Game *g);

    std::vector<Button *> getButtons();

    virtual void show();

    virtual void onPressed(Button *b);

    virtual void update(int64_t time);

    virtual ~Menu();

   protected:
    std::vector<Button *> buttons;
    Game *game;
};