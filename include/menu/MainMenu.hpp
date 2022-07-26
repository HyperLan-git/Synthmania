#pragma once

class MainMenu;

#include "Menu.hpp"

class MainMenu : public Menu {
   public:
    MainMenu(Game *g);

    virtual void show();

    virtual void onPressed(Button *b);

    virtual void update(int64_t time);

    virtual ~MainMenu();
};