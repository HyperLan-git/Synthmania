#pragma once

class ScoreScreen;

#include "Menu.hpp"
#include "Score.hpp"
#include "Synthmania.hpp"

class ScoreScreen : public Menu {
   public:
    ScoreScreen(Game& game);

    bool saveReplay(std::string path);

    virtual void show() override;

    virtual void onPressed(const std::shared_ptr<Button>& b) override;

    virtual ~ScoreScreen();

   private:
    Score score;
    uint64_t replayStart;
    std::vector<Message> replay;
    uint32_t guisKept;
};