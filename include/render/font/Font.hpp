#pragma once

#include <string>
#include <vector>

#include "ImageView.hpp"

struct Character {
    int offsetLeft, offsetTop;
    uint width, height;
    ulong character;
    ImageView* texture;
};

struct Font {
    std::string name;
    std::map<ulong, Character> characters;
};