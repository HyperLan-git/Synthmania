#include "Note.hpp"

int getDifferenceFromC4(u_char pitch) {
    char octave = pitch / 12;
    octave -= 5;
    int result = octave * 7;
    u_char key = pitch % 12;
    result += key;
    if (key > 0) result--;
    if (key > 2) result--;
    if (key > 5) result--;
    if (key > 7) result--;
    if (key > 9) result--;
    return result;
}
#include <iostream>
ImageView* getTextureForNote(std::vector<ImageView*> textures, u_char pitch,
                             float duration, Key currentKey) {
    float initial = 1;
    std::string texName = "note_";
    while (initial > duration) initial /= 2;
    if (initial < 1)
        texName += std::to_string((int)(1.0 / initial)) +
                   "th";  // Yeah secondth deal with it
    else
        texName += std::to_string((int)initial);
    std::cout << duration << " " << texName << std::endl;

    return getTextureByName(textures, texName.c_str());
}

glm::vec2 getSizeAndLocForNote(float duration) {
    int i = duration;
    if (duration < 1.f) i = -(1.f) / duration;
    switch (i) {
        case -4:
            return {0.36f, 0.53f};
        case 1:
            return {0.5f, 0.25f};
        default:
            return {0.36f, 0.53f};
    }
}

Note::Note(const char* name, float time, u_char pitch, float duration,
           std::vector<ImageView*> textures)
    : Gui(getTextureForNote(textures, pitch, duration, Key::SOL), name) {
    this->time = time;
    std::cout << time << std::endl;
    glm::vec2 temp = getSizeAndLocForNote(duration);
    this->position.y = temp.x - 0.083f * getDifferenceFromC4(pitch);
    this->size = {temp.y, temp.y};
    this->pitch = pitch;
}

ShaderData* Note::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    edata->pos = position;
    edata->rot = rotation;
    edata->size = size;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

bool Note::update(float time) {
    this->position.x = (this->time - time) * 2 - 1.4f;
    return time > this->time;
}
