#pragma once

class Checkbox;

#include "MenuElement.hpp"

// Twitter blue checkmark
class Checkbox : public MenuElement {
   public:
    Checkbox(ImageView *unchecked, ImageView *checked, std::string name,
             bool isChecked = false);

    virtual void onClicked(glm::vec2 pos);

    bool isChecked();
    void check(bool value = true);

   private:
    bool checked;
    ImageView *uncheckedTex, *checkedTex;
};