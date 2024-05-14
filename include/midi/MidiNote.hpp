#pragma once
#include <bits/functional_hash.h>
#include <sys/types.h>

#include <cstdint>
#include <vector>

// WTF midi standard???
enum MidiPercussion {
    KICK = 36,
    SNARE = 38,
    HAT = 42,
    HAT_CLOSE = 44,
    TOM_LOW = 45,
    HAT_OPEN = 46,
    TOM_MID = 48,
    CRASH = 49,
    TOM_HIGH = 50,
    RIDE = 51
};

struct MidiNote {
    uint64_t timestamp;
    uint64_t length;
    unsigned char note;
    unsigned char velocity;
};

struct KeySignature {
    short sharps;  // Below 0 means flats
    bool minor;    // Natural minor
};

struct TrackPartition {
    bool drumming;
    uint64_t MPQ;
    std::vector<MidiNote> notes;
    KeySignature signature = {0, false};
};

template <>
struct std::hash<MidiNote> {
    size_t operator()(MidiNote const &note) const;
};