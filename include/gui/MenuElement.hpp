#pragma once

class MenuElement;

#include "Gui.hpp"

class MenuElement : public Gui {
   public:
    MenuElement(ImageView* texture, std::string name);

    virtual bool isInside(glm::vec2 position);

    virtual void onPressed(glm::vec2 pos);

    virtual void onClicked(glm::vec2 pos);

    virtual ~MenuElement();
};