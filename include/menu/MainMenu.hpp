#pragma once

class MainMenu;

#include "Menu.hpp"

class MainMenu : public Menu {
   public:
    MainMenu(Game *g);

    virtual void show() override;

    virtual void onPressed(const std::shared_ptr<Button> &b) override;

    virtual void update(int64_t time) override;

    virtual ~MainMenu();
};