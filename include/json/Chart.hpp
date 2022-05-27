#pragma once
#include <string>
#include <vector>

struct Diff {
    std::string name, midi;
    uint track_id;
};

struct Chart {
    std::vector<Diff> diffs;
    std::string name, artist, audio, animation;
    uint64_t offset;
};