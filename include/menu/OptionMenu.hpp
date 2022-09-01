#pragma once

class OptionMenu;

#include "Menu.hpp"
#include "Renderer.hpp"
#include "Slider.hpp"

class OptionMenu : public Menu {
   public:
    OptionMenu(Game* game);

    virtual void show();

    ~OptionMenu();

   private:
};