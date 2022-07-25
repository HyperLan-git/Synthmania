#include "Judgement.hpp"

Judgement::Judgement(const char* name, std::vector<ImageView*> textures,
                     TrackPartition notes)
    : Gui(getTextureByName(textures, "judgement_line"), name) {
    this->partition = notes;
    if (this->partition.notes.empty()) return;
    auto n = this->partition.notes;
    this->position.y = 0.25 - 0.083f * getDifferenceFromC4(n[0].note);
}

bool Judgement::update(int64_t time) {
    if (this->partition.notes.empty()) return false;
    auto notes = this->partition.notes;
    MidiNote prev = (notes[0]), next = (notes[notes.size() - 1]);
    if (time < 0) {
        this->position.y = 0.25 - 0.083f * getDifferenceFromC4(prev.note);
        return false;
    }
    for (MidiNote note : notes) {
        if (note.timestamp > prev.timestamp && note.timestamp < time)
            prev = note;
        else if (note.timestamp < next.timestamp && note.timestamp > time)
            next = note;
    }
    double progress = (next.timestamp - prev.timestamp);
    if (progress > 0) progress = (time - prev.timestamp) / progress;
    this->position.y =
        0.25 + progress * -0.083f * getDifferenceFromC4(next.note) +
        (1 - progress) * -0.083f * getDifferenceFromC4(prev.note);

    return false;
}
