#pragma once

class Button;

#include "Gui.hpp"

class Button : public Gui {
   public:
    Button(ImageView* texture, ImageView* pressedTexture, const char* name,
           glm::vec2 position, glm::vec2 size);

    bool isPressed();

    virtual bool isInside(glm::vec2 position);

    virtual void onPressed();
    virtual void onReleased();

    virtual ~Button();

   private:
    ImageView *normalTexture, *pressedTexture;
    bool pressed = false;
};