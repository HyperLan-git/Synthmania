#include "PartitionNotation.hpp"

float getNotationPosition(int64_t time, int64_t currentTime) {
    return (time - currentTime) / 300000.f - 1.3f;
}

int getDifferenceFromC4(unsigned char pitch, KeySignature signature) {
    char octave = pitch / 12;
    octave -= 5;
    int result = octave * 7;
    unsigned char key = pitch % 12;
    result += key;
    signature = toMajor(signature);
    short sharps = signature.sharps;
    if (key > 0) result--;
    if (key == 1 && sharps < -3) result++;
    if (key > 2) result--;
    if (key == 3 && sharps < -1) result++;
    if (key == 4 && sharps < -2) result++;
    if (key > 5) result--;
    if (key == 6 && sharps < -4) result++;
    if (key > 7) result--;
    if (key == 8 && sharps < -2) result++;
    if (key > 9) result--;
    if (key == 10 && sharps < 0) result++;
    if (key == 11 && sharps < -6) result++;
    return result;
}

KeySignature toMajor(const KeySignature& signature) {
    if (!signature.minor) return signature;
    short sharps = signature.sharps - 3;
    if (sharps < -7) sharps += 15;
    return KeySignature{.sharps = sharps, .minor = false};
}

Accidental getAccidental(unsigned char pitch, KeySignature signature) {
    signature = toMajor(signature);
    unsigned char key = pitch % 12;
    switch (key) {
        case 0:
            if (signature.sharps >= 1 || signature.sharps <= -6) return NATURAL;
            return NONE;
        case 1:
            if (signature.sharps >= 2 || signature.sharps <= -4) return NONE;
            return SHARP;
        case 2:
            if (signature.sharps >= 4 || signature.sharps <= -4) return NATURAL;
            return NONE;
        case 3:
            if (signature.sharps >= 4 || signature.sharps <= -2) return NONE;
            return SHARP;
        case 4:
            if (signature.sharps >= 6 || signature.sharps <= -2) return NATURAL;
            return NONE;
        case 5:
            if (signature.sharps >= 1 || signature.sharps <= -7) return NATURAL;
            return NONE;
        case 6:
            if (signature.sharps >= 1 || signature.sharps <= -5) return NONE;
            return SHARP;
        case 7:
            if (signature.sharps >= 3 || signature.sharps <= -5) return NATURAL;
            return NONE;
        case 8:
            if (signature.sharps >= 3 || signature.sharps <= -3) return NONE;
            return SHARP;
        case 9:
            if (signature.sharps >= 5 || signature.sharps <= -3) return NATURAL;
            return NONE;
        case 10:
            if (signature.sharps >= 5 || signature.sharps <= -1) return NONE;
            return SHARP;
        case 11:
            if (signature.sharps >= 7 || signature.sharps <= -1) return NATURAL;
            return NONE;
    }
    return NONE;
}

bool isFromCMajor(unsigned char pitch) {
    unsigned char key = pitch % 12;
    return !(key == 1 || key == 3 || key == 6 || key == 8 || key == 10);
}

int getOffset(Key key) {
    switch (key) {
        case FA:
            return 8;
    }
    return 0;
}

PartitionNotation::PartitionNotation(std::string name, int64_t time,
                                     unsigned char pitch, Texture texture,
                                     Key key, KeySignature signature)
    : Gui(texture, name) {
    this->time = time;
    this->position.y = 0.5f - 0.083f * (getDifferenceFromC4(pitch, signature) +
                                        getOffset(key));
}

PartitionNotation::PartitionNotation(std::string name, int64_t time,
                                     double verticalPosition, Texture texture)
    : Gui(texture, name) {
    this->time = time;
    this->position.y = verticalPosition;
}

int64_t PartitionNotation::getTime() { return time; }

bool PartitionNotation::update(int64_t time) {
    this->position.x = getNotationPosition(this->time, time);
    return false;
}