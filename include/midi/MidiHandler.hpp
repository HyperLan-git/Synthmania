#pragma once

class MidiHandler;
struct Message;

#define LIBREMIDI_HEADER_ONLY 1
#define LIBREMIDI_ALSA 1

#include <boost/lockfree/queue.hpp>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <libremidi/libremidi.hpp>
#include <libremidi/reader.hpp>
#include <mutex>
#include <queue>

#include "MidiNote.hpp"

template <>
struct std::hash<libremidi::message> {
    size_t operator()(libremidi::message const& message) const;
};

struct Message {
    libremidi::message_type type;
    int channel;
    unsigned char data1;
    unsigned char data2;
    uint64_t timestamp;
};

class MidiHandler {
   public:
    MidiHandler();
    bool hasMessage();
    Message getMessage();
    std::vector<MidiNote> readMidi(const char* path);
    ~MidiHandler();

   private:
    boost::lockfree::queue<Message> messages{10};
    uint64_t start_time;
};

uint64_t micros();