#pragma once

class MenuElement;

#include "Gui.hpp"

class MenuElement : public Gui {
   public:
    MenuElement(ImageView* texture, std::string name);

    void focus(bool focused = true);

    virtual bool isInside(glm::vec2 position);

    virtual void onPressed(glm::vec2 pos);

    virtual void onClicked(glm::vec2 pos);

    virtual void onKeyPressed(int key, int scancode, int mods);

    virtual void onTyped(unsigned int character);

    virtual ~MenuElement();

   protected:
    bool selected = false;
};