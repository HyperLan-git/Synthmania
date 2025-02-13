#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Diff {
    std::string name, midi;
    unsigned int track_id;
};

struct Chart {
    std::vector<Diff> diffs;
    std::string name, artist, audio, animation, plugin, plugindata;
    uint64_t offset;
};