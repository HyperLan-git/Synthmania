#include "Note.hpp"

ImageView* getTextureForNote(std::vector<ImageView*> textures, u_char pitch,
                             float duration, Key currentKey) {
    float initial = 8;
    std::string texName = "note_";
    while (initial > duration) initial /= 2;
    if (initial < 1)
        texName += std::to_string((int)(1.0 / initial)) +
                   "th";  // Yeah secondth deal with it
    else
        texName += (int)initial;

    return getTextureByName(textures, texName.c_str());
}

Note::Note(const char* name, float time, u_char pitch, float duration,
           std::vector<ImageView*> textures)
    : Gui(getTextureForNote(textures, pitch, duration, Key::SOL), name) {
    this->time = time;
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

bool Note::update(float time) { return time > this->time; }