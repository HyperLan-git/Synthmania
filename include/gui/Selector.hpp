#pragma once

class Selector;

#include "Font.hpp"
#include "Game.hpp"
#include "MenuElement.hpp"

// XXX find a way to have gui with subparts without having to add them all one
// by one
class Selector : public MenuElement {
   public:
    Selector(Texture texture, Texture selectTexture, Game& game,
             std::string name, std::vector<std::string> elements,
             double textSize, std::string font, int textLimit);

    virtual bool isInside(glm::vec2 position);

    virtual void onClicked(glm::vec2 pos);

    void recalculatePositions();

    std::vector<std::shared_ptr<Gui>> getGuis();

    int getSelected();
    void select(int selected);

    virtual ~Selector() = default;

   private:
    std::vector<std::vector<std::pair<Text, std::shared_ptr<Gui>>>> strings;
    std::vector<std::shared_ptr<Gui>> buttons;
    int current;
    bool opened;
    int textLimit;
};