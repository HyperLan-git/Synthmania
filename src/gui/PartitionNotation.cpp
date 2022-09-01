#include "PartitionNotation.hpp"

int getDifferenceFromC4(unsigned char pitch) {
    char octave = pitch / 12;
    octave -= 5;
    int result = octave * 7;
    unsigned char key = pitch % 12;
    result += key;
    if (key > 0) result--;
    if (key > 2) result--;
    if (key > 5) result--;
    if (key > 7) result--;
    if (key > 9) result--;
    return result;
}

bool isFromCMajor(unsigned char pitch) {
    unsigned char key = pitch % 12;
    return !(key == 1 || key == 3 || key == 6 || key == 8 || key == 10);
}

PartitionNotation::PartitionNotation(std::string name, int64_t time,
                                     unsigned char pitch, ImageView* texture)
    : Gui(texture, name) {
    this->time = time;
    this->position.y = 0.5f - 0.083f * getDifferenceFromC4(pitch);
}

PartitionNotation::PartitionNotation(std::string name, int64_t time,
                                     double verticalPosition,
                                     ImageView* texture)
    : Gui(texture, name) {
    this->time = time;
    this->position.y = verticalPosition;
}

int64_t PartitionNotation::getTime() { return time; }

bool PartitionNotation::update(int64_t time) {
    this->position.x = (this->time - time) / 300000. - 1.4f;
    return false;
}