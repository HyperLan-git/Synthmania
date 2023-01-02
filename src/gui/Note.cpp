#include "Note.hpp"

#define A_32TH .03125f

#define ALPHA_HIT .85

std::vector<double> splitDuration(double duration) {
    if (duration <= A_32TH) return {duration};
    std::vector<double> result;
    while (duration > A_32TH) {
        double d = 2;
        while (d > duration) d /= 2;
        duration -= d;
        result.push_back(d);
    }
    return result;
}

ImageView* getTextureForNote(std::vector<ImageView*> textures,
                             unsigned char pitch, double duration,
                             Key currentKey) {
    if (currentKey == Key::DRUM) {
        switch (pitch) {
            case MidiPercussion::CRASH:
            case MidiPercussion::HAT:
            case MidiPercussion::RIDE:
                return getTextureByName(textures, "note_cross");
            default:
                return getTextureByName(textures, "note_4th");
        }
    }
    float initial = 2;
    std::string texName = "note_";
    while (initial > duration) initial /= 2;
    // std::cout << duration << " " << (int)pitch << std::endl;
    if (duration < A_32TH) initial = A_32TH;

    if (initial < 1)
        texName += std::to_string((int)(1.0 / initial)) +
                   "th";  // Yeah secondth deal with it
    else
        texName += std::to_string((int)initial);

    return getTextureByName(textures, texName.c_str());
}

glm::vec2 getSizeAndLocForNote(double duration, Key k, unsigned char pitch) {
    int i = duration;
    if (duration < A_32TH) duration = A_32TH;
    if (duration < 1.f) i = -(1.f) / duration;
    // TODO put all that in the skin file
    if (i < 0) return {-0.14f, 0.53f};
    return {0, 0.25f};
}

unsigned char transposePitch(Key k, unsigned char pitch) {
    if (k == Key::DRUM) switch (pitch) {
            case MidiPercussion::KICK:
                return 65;
            case MidiPercussion::SNARE:
                return 72;
            case MidiPercussion::RIDE:
                return 77;
            case MidiPercussion::CRASH:
                return 81;
            case MidiPercussion::HAT:
                return 79;
            case MidiPercussion::TOM_HIGH:
                return 76;
            case MidiPercussion::TOM_MID:
                return 74;
            case MidiPercussion::TOM_LOW:
                return 69;
            default:
                return 60;
        }
    return pitch;
}

Note::Note(std::string name, int64_t time, unsigned char pitch,
           double totalDuration, double duration, uint64_t MPQ,
           std::vector<ImageView*> textures, Key key)
    : PartitionNotation(name, time, transposePitch(key, pitch),
                        getTextureForNote(textures, pitch, duration, key),
                        key) {
    this->totalDuration = totalDuration * MPQ * 4;
    this->duration = duration * MPQ * 4;
    this->kill_moment =
        time + (key == Key::DRUM ? DRUM_HIT_WINDOW : HIT_WINDOW);
    glm::vec2 temp = getSizeAndLocForNote(duration, key, pitch);
    GraphicalEffect* e =
        new GraphicalEffect(applyOffset, new float[2]{0, temp.x});
    this->effects.push_back(e);
    this->size = {temp.y, temp.y};
    this->pitch = pitch;
}

void Note::setStatus(NoteStatus status) {
    this->status = status;
    switch (status) {
        case WAITING:
            color = {1, 1, 1, 1};
            break;
        case HIT:
            negate = 1;
            color = {.1, .5, .1, ALPHA_HIT};
            break;
        case FINISHED:
            negate = 1;
            color = {0, 1, 0, ALPHA_HIT};
            break;
        default:
            negate = 1;
            color = {1, 0, 0, ALPHA_HIT};
    }
}

NoteStatus Note::getStatus() { return status; }

int64_t Note::getTime() { return this->time; }

int64_t Note::getDuration() { return this->duration; }

int64_t Note::getTotalDuration() { return this->totalDuration; }

unsigned char Note::getPitch() { return pitch; }

void Note::kill(uint64_t moment) { this->kill_moment = moment; }

bool Note::justMissed() { return missed; }

bool Note::update(int64_t time) {
    PartitionNotation::update(time);
    if (missed) missed = false;
    if (this->status == WAITING && kill_moment < time) {
        missed = true;
    }

    if (this->kill_moment < time) {
        this->color.a =
            (1. + (this->kill_moment - time) / (double)DELETE_ANIM) * ALPHA_HIT;
    }

    return (int64_t)(kill_moment + DELETE_ANIM) <= time;
}