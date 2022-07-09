#pragma once

#include "Gui.hpp"

class TemporaryGui : public Gui {
   public:
    TemporaryGui(ImageView* texture, const char* name, int64_t time,
                 int64_t death);

    void setFinalPosition(glm::vec2 pos);
    void setPosition(glm::vec2 pos);

    virtual bool update(int64_t time);

   protected:
    glm::vec2 finalPosition = {0, -0.2}, initialPosition = getPosition();
    int64_t time, death;
};