#pragma once

class OptionMenu;

#include <fstream>

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

    void save();

    ~OptionMenu();

   private:
    Slider *volume, *music, *synth;
    TextArea *audioLatency, *graphicLatency, *audioLeniency, *bufSize, *bufAmt,
        *pluginFolders;
    Selector *midiDevice, *audioDevice, *notation, *skin;
    Checkbox* fullscreen;
    Options* options;
};