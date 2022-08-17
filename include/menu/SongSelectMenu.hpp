#pragma once

class SongSelectMenu;

#include "Menu.hpp"
#include "Synthmania.hpp"

class SongSelectMenu : public Menu {
   public:
    SongSelectMenu(Game *g, std::string folder);

    virtual void show();

    virtual void onPressed(Button *b);

    virtual void update(int64_t time);

    virtual ~SongSelectMenu();

   private:
    std::vector<std::string> songs;
    std::string folder;
};