#pragma once
#include <bits/functional_hash.h>
#include <sys/types.h>

#include <cstdint>
#include <stdexcept>
#include <vector>

#define MICROS_32TH 31250

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
    uint64_t timestamp;
    short sharps;  // Below 0 means flats
    bool minor;    // Natural minor
};

struct TimeSignature {
    uint64_t timestamp;
    unsigned char numerator;
    unsigned char denominatorPower;
    unsigned char ticks_per_beat;
    unsigned char
        notes_per_beat;  // 32nd notes (triple quavers) per beat, mostly useless

    unsigned int getDenominator() const { return (1u << denominatorPower); }
    uint64_t getMicrosPerMeasure(uint64_t MPQ) const {
        uint64_t result = MPQ;
        result *= numerator;
        return (result >> (denominatorPower)) << 2;
    }
};

struct TempoChange {
    uint64_t timestamp;
    uint64_t MPQ;

    double getBPM() const { return (60 * 1000000.) / MPQ; }
};

constexpr KeySignature DEFAULT_KEY{0, 0, false};
constexpr TimeSignature DEFAULT_TIME_SIGNATURE{0, 4, 2, 24, 8};
constexpr TempoChange DEFAULT_TEMPO{0, 500000};

template <typename T>
const T& getLastBefore(const std::vector<T>& events, uint64_t time) {
    if (events.empty())
        throw std::runtime_error("Cannot BSearch an empty array !");
    std::size_t idx = 0, idx2 = events.size() - 1;
    std::size_t mid = (idx + idx2) / 2;
    while (idx + 1 < idx2) {
        if (events[mid].timestamp > time) {
            idx2 = mid;
        } else {
            idx = mid;
        }
        mid = (idx + idx2) / 2;
    }
    if (events[idx2].timestamp <= time) {
        return events[idx2];
    }
    if (idx2 > 0) return events[idx2 - 1];
    return events[0];
}

struct TrackPartition {
    bool drumming;
    std::vector<MidiNote> notes;
    std::vector<TimeSignature> timeSignatures;
    std::vector<TempoChange> tempoChanges;
    std::vector<KeySignature> keyChanges;

    const TempoChange& getTempoAt(uint64_t time) const {
        return getLastBefore(tempoChanges, time);
    }

    const TimeSignature& getTimeSignatureAt(uint64_t time) const {
        return getLastBefore(timeSignatures, time);
    }

    const KeySignature& getKeySignatureAt(uint64_t time) const {
        return getLastBefore(keyChanges, time);
    }
};

template <>
struct std::hash<MidiNote> {
    size_t operator()(MidiNote const& note) const;
};