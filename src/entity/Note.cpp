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

bool isFromCMajor(u_char pitch) {
    u_char key = pitch % 12;
    return !(key == 1 || key == 3 || key == 6 || key == 8 || key == 10);
}

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
    // std::cout << duration << " " << texName << std::endl;

    return getTextureByName(textures, texName.c_str());
}

glm::vec2 getSizeAndLocForNote(float duration) {
    int i = duration;
    if (duration < 1.f) i = -(1.f) / duration;
    switch (i) {
        case -4:
        case -2:
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
    this->kill_moment = time + HIT_WINDOW;
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
    edata->color = color;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

void Note::setStatus(NoteStatus status) {
    this->status = status;
    if (status == HIT)
        color = {0, 1, 0, 1};
    else if (status == MISSED)
        color = {1, 0, 0, 1};
    else
        color = {1, 1, 1, 1};
}

bool Note::update(float time) {
    this->position.x = (this->time - time) * 2 - 1.4f;
    if (this->status == WAITING && (kill_moment) < time) setStatus(MISSED);

    if (this->kill_moment < time)
        this->color.a = 1 + (this->kill_moment - time) / DELETE_ANIM;

    return kill_moment + DELETE_ANIM <= time;
}
