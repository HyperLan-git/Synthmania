#include "Judgement.hpp"

Judgement::Judgement(std::string name, Texture texture, TrackPartition notes)
    : Gui(texture, name) {
    this->partition = notes;
    if (this->partition.drumming) {
        this->position.y = 0;
        return;
    }
    if (this->partition.notes.empty()) return;
    auto n = this->partition.notes;
    KeySignature sig = this->partition.keyChanges.empty()
                           ? DEFAULT_KEY
                           : this->partition.getKeySignatureAt(n[0].timestamp);
    this->position.y = 0.25 - 0.083f * getDifferenceFromC4(n[0].note, sig);
}

bool Judgement::update(int64_t time) {
    if (this->partition.notes.empty() || this->partition.drumming) return false;
    auto& notes = this->partition.notes;
    MidiNote prev = (notes[0]), next = (notes[notes.size() - 1]);
    KeySignature sig = this->partition.keyChanges.empty()
                           ? DEFAULT_KEY
                           : this->partition.getKeySignatureAt(time);
    if (time < 0) {
        this->position.y = 0.25 - 0.083f * getDifferenceFromC4(prev.note, sig);
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
        0.25 + progress * -0.083f * getDifferenceFromC4(next.note, sig) +
        (1 - progress) * -0.083f * getDifferenceFromC4(prev.note, sig);

    return false;
}
