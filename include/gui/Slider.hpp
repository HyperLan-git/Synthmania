#pragma once

class Slider;

#include "MenuElement.hpp"

class Slider : public MenuElement {
   public:
    Slider(Texture texture, std::string name, float width = .5f, float min = 0,
           float max = 1, bool vertical = false);

    float getValue();
    void setValue(float value);

    virtual bool isInside(glm::vec2 pos);

    virtual void onPressed(glm::vec2 pos);

    virtual void onClicked(glm::vec2 pos);

    virtual ShaderData getShaderData() const;

    virtual ~Slider();

   private:
    float min, max, pos, width;
    bool vertical;
};