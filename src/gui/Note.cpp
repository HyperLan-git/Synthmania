#include "Note.hpp"

#define A_32TH .03125f

#define ALPHA_HIT .85

std::vector<double> splitDuration(double duration) {
    if (duration <= A_32TH) return {duration};
    std::vector<double> result;
    while (duration > A_32TH) {
        double d = 1;
        while (d > duration) d /= 2;
        duration -= d;
        result.push_back(d);
    }
    return result;
}

Texture getTextureForNote(unsigned char pitch, double duration, Key currentKey,
                          bool linked) {
    if (currentKey == Key::DRUM) {
        switch (pitch) {
            case MidiPercussion::CRASH:
            case MidiPercussion::HAT:
            case MidiPercussion::RIDE:
                return Texture("note_cross");
            default:
                return Texture("note_4th");
        }
    }
    if (linked) return Texture("note_4th");
    float initial = 1;
    std::string texName = "note_";
    while (initial > duration) initial /= 2;
    if (duration < A_32TH) initial = A_32TH;

    if (initial < 1)
        texName += std::to_string((int)(1.0 / initial)) +
                   "th";  // Yeah secondth deal with it
    else
        texName += std::to_string((int)initial);

    return Texture(texName);
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

bool shouldFlip(unsigned char pitch) { return pitch < 73; }

Note::Note(std::string name, int64_t time, unsigned char pitch,
           double totalDuration, double duration, uint64_t MPQ, Key key,
           KeySignature signature, bool linked)
    : PartitionNotation(name, time, transposePitch(key, pitch),
                        getTextureForNote(pitch, duration, key, linked), key,
                        signature) {
    unsigned char tPitch = transposePitch(key, pitch);
    bool flip = key == Key::DRUM && shouldFlip(tPitch);
    this->totalDuration = totalDuration * MPQ * 4;
    this->duration = duration * MPQ * 4;
    this->kill_moment =
        time + (key == Key::DRUM ? DRUM_HIT_WINDOW : HIT_WINDOW);
    this->pitch = pitch;
    glm::vec2 temp = getSizeAndLocForNote(duration, key, pitch);
    this->effects.emplace_back(
        applyOffset, std::initializer_list<float>{0, flip ? -temp.x : temp.x});
    this->size = {temp.y, flip ? -temp.y : temp.y};
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

NoteStatus Note::getStatus() const { return status; }

int64_t Note::getTime() const { return this->time; }

int64_t Note::getDuration() const { return this->duration; }

int64_t Note::getTotalDuration() const { return this->totalDuration; }

unsigned char Note::getPitch() const { return pitch; }

void Note::kill(uint64_t moment) { this->kill_moment = moment; }

bool Note::justMissed() const { return missed; }

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