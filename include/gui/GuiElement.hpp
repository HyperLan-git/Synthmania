#pragma once

class GuiElement;

#include "Gui.hpp"

class GuiElement : public Gui {
   public:
    GuiElement(ImageView* texture, std::string name);

    virtual bool isInside(glm::vec2 position);

    virtual void onPressed(glm::vec2 pos) = 0;

    virtual ~GuiElement();
};