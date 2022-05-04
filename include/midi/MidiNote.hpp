#pragma once
#include <bits/functional_hash.h>
#include <sys/types.h>

#include <cstdint>

struct MidiNote;

struct MidiNote {
    uint64_t timestamp;
    uint64_t length;
    u_char note;
    u_char velocity;
};

template <>
struct std::hash<MidiNote> {
    size_t operator()(MidiNote const &note) const;
};