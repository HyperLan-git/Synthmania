#pragma once

class SongSelectMenu;

#include <filesystem>

#include "Menu.hpp"
#include "Synthmania.hpp"

class SongSelectMenu : public Menu {
   public:
    SongSelectMenu(Game* g, std::string folder);

    virtual void show() override;

    virtual void onPressed(const std::shared_ptr<Button>& b) override;

    virtual void update(int64_t time) override;

    virtual ~SongSelectMenu();

   private:
    std::vector<std::string> songs;
    std::string folder;
};