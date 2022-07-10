#pragma once

#include <string>
#include <vector>

#include "ImageView.hpp"

// This represents relevant data to position a character
struct Character {
    int offsetLeft, offsetTop;
    unsigned int width, height, advance;
    ulong character;
    ImageView* texture;
};

struct Font {
    std::string name;
    std::map<ulong, Character> characters;
};

// This represents everything a character knows in order to be rendered
struct Text {
    glm::vec2 pos, size;
    Character character;
};