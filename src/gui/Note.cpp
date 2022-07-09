#include "Note.hpp"

std::vector<double> splitDuration(double duration) {
    if (duration <= 0.125) return {duration};
    std::vector<double> result;
    while (duration > 0.125) {
        double d = 2;
        while (d > duration) d /= 2;
        duration -= d;
        result.push_back(d);
    }
    return result;
}

ImageView* getTextureForNote(std::vector<ImageView*> textures, u_char pitch,
                             double duration, Key currentKey) {
    float initial = 2;
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
    if (duration < 0.125f) duration = 0.125f;
    if (duration < 1.f) i = -(1.f) / duration;
    // TODO put all that in the skin file
    if (i < 0) return {-0.14f, 0.53f};
    return {0, 0.25f};
}

Note::Note(const char* name, int64_t time, u_char pitch, double totalDuration,
           double duration, uint64_t MPQ, std::vector<ImageView*> textures)
    : PartitionNotation(
          name, time, pitch,
          getTextureForNote(textures, pitch, duration, Key::SOL)) {
    this->totalDuration = totalDuration * MPQ * 4;
    this->duration = duration * MPQ * 4;
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
    switch (status) {
        case WAITING:
            color = {1, 1, 1, 1};
            break;
        case HIT:
            color = {.1, .5, .1, 1};
            break;
        case FINISHED:
            color = {0, 1, 0, 1};
            break;
        default:
            color = {1, 0, 0, 1};
    }
}

NoteStatus Note::getStatus() { return status; }

int64_t Note::getTime() { return this->time; }

int64_t Note::getDuration() { return this->duration; }

int64_t Note::getTotalDuration() { return this->totalDuration; }

u_char Note::getPitch() { return pitch; }

void Note::kill(uint64_t moment) { this->kill_moment = moment; }

bool Note::justMissed() { return missed; }

bool Note::update(int64_t time) {
    PartitionNotation::update(time);
    if (missed) missed = false;
    if (this->status == WAITING && kill_moment < time) {
        missed = true;
    }

    if (this->kill_moment < time) {
        this->color.a = 1. + (this->kill_moment - time) / (double)DELETE_ANIM;
    }

    return (int64_t)(kill_moment + DELETE_ANIM) <= time;
}