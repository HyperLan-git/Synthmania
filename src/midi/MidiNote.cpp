#include "MidiNote.hpp"

size_t std::hash<MidiNote>::operator()(MidiNote const &note) const {
    return ((note.note << 7) | (note.velocity << 5)) ^
           (int)(666556507 * (note.timestamp + 5)) ^
           (int)(note.length * 89891899);
}