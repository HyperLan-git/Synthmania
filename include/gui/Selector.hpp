#pragma once

class Selector;

#include "Font.hpp"
#include "MenuElement.hpp"
#include "Renderer.hpp"

// TODO find a way to have gui with subparts without having to add them all one
// by one
class Selector : public MenuElement {
   public:
    Selector(ImageView* texture, ImageView* selectTexture, Game* game,
             std::string name, std::vector<std::string> elements,
             double textSize, std::string font, int textLimit);

    virtual bool isInside(glm::vec2 position);

    virtual void onClicked(glm::vec2 pos);

    void recalculatePositions();

    std::vector<Gui*> getGuis();

    int getSelected();
    void select(int selected);

    virtual ~Selector() = default;

   private:
    std::vector<std::vector<std::pair<Text, Gui*>>> strings;
    std::vector<Gui*> buttons;
    int current;
    bool opened;
    int textLimit;
};