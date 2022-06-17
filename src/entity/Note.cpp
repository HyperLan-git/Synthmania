#include "Note.hpp"

ImageView* getTextureForNote(std::vector<ImageView*> textures, u_char pitch,
                             double duration, Key currentKey) {
    float initial = 1;
    std::string texName = "note_";
    while (initial > duration) initial /= 2;
    // std::cout << duration << " " << (int)pitch << std::endl;
    if (duration == 0) initial = 0.125f;

    if (initial < 1)
        texName += std::to_string((int)(1.0 / initial)) +
                   "th";  // Yeah secondth deal with it
    else
        texName += std::to_string((int)initial);

    return getTextureByName(textures, texName.c_str());
}

glm::vec2 getSizeAndLocForNote(double duration) {
    int i = duration;
    if (duration < 1.f) i = -(1.f) / duration;
    switch (i) {
        case -8:
        case -4:
        case -2:
            return {-0.14f, 0.53f};
        case 1:
        case 2:
            return {0, 0.25f};
        default:
            return {-0.14f, 0.53f};
    }
}

Note::Note(const char* name, int64_t time, u_char pitch, double duration,
           std::vector<ImageView*> textures)
    : PartitionNotation(
          name, time, pitch,
          getTextureForNote(textures, pitch, duration, Key::SOL)) {
    this->duration = duration * 350000;
    this->kill_moment = time + HIT_WINDOW;
    glm::vec2 temp = getSizeAndLocForNote(duration);
    this->graphicalPosition.y = temp.x;
    this->size = {temp.y, temp.y};
    this->pitch = pitch;
}

ShaderData* Note::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    glm::vec3 p = glm::vec3(graphicalPosition, 0);
    p += position;
    edata->pos = p;
    edata->rot = rotation;
    edata->size = size;
    edata->color = color;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

void Note::setStatus(NoteStatus status) {
    this->status = status;
    if (status == WAITING) {
        color = {1, 1, 1, 1};
        return;
    }
    if (status == HIT)
        color = {0, 1, 0, 1};
    else
        color = {1, 0, 0, 1};
}

NoteStatus Note::getStatus() { return status; }

int64_t Note::getTime() { return this->time; }

int64_t Note::getDuration() { return this->duration; }

u_char Note::getPitch() { return pitch; }

void Note::kill(uint64_t moment) { this->kill_moment = moment; }

bool Note::update(int64_t time) {
    PartitionNotation::update(time);
    if (this->status == WAITING && kill_moment < time) setStatus(MISSED);

    if (this->kill_moment < time) {
        this->color.a = 1. + (this->kill_moment - time) / (double)DELETE_ANIM;
    }

    return (int64_t)(kill_moment + DELETE_ANIM) <= time;
}