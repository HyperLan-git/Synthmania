#pragma once

class Button;

#include "MenuElement.hpp"

class Button : public MenuElement {
   public:
    Button(ImageView* texture, ImageView* pressedTexture, std::string name,
           glm::vec2 position, glm::vec2 size);

    bool isPressed();

    virtual bool isInside(glm::vec2 position);

    virtual void onPressed(glm::vec2 pos);
    virtual void onReleased();

    virtual ~Button();

   private:
    ImageView *normalTexture, *pressedTexture;
    bool pressed = false;
};