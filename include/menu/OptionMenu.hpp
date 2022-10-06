#pragma once

class OptionMenu;

#include "Checkbox.hpp"
#include "Menu.hpp"
#include "Renderer.hpp"
#include "Selector.hpp"
#include "Slider.hpp"
#include "TextArea.hpp"

class OptionMenu : public Menu {
   public:
    OptionMenu(Game* game);

    virtual void show();

    virtual void onPressed(Button* b);

    ~OptionMenu();

   private:
    TextArea *audioLatency, *graphicLatency, *audioLeniency;
    Checkbox* fullscreen;
};