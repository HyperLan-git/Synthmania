#pragma once
#include <bits/functional_hash.h>
#include <sys/types.h>

#include <cstdint>
#include <vector>

struct MidiNote {
    uint64_t timestamp;
    uint64_t length;
    unsigned char note;
    unsigned char velocity;
};

struct TrackPartition {
    uint64_t MPQ;
    std::vector<MidiNote> notes;
};

template <>
struct std::hash<MidiNote> {
    size_t operator()(MidiNote const &note) const;
};