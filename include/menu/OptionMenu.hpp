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

    virtual void show() override;

    virtual void onPressed(const std::shared_ptr<Button>& b) override;

    void save();

    ~OptionMenu();

   private:
    std::shared_ptr<Slider> volume, music, synth;
    std::shared_ptr<TextArea> audioLatency, graphicLatency, audioLeniency,
        bufSize, bufAmt, pluginFolders;
    std::shared_ptr<Selector> midiDevice, audioDevice, notation, skin;
    std::shared_ptr<Checkbox> fullscreen;
    Options* options;
};