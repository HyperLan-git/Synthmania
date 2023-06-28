#pragma once

#include "ImageView.hpp"

// This represents relevant data to position a character
struct Character {
    int offsetLeft, offsetTop;
    unsigned int width, height, advance, vAdvance;
    unsigned long character;
    ImageView* texture;
};

struct Font {
    std::string name;
    std::map<unsigned long, Character> characters;
};

// This represents everything a character needs in order to be rendered
struct Text {
    glm::vec2 pos, size;
    Character character;
};