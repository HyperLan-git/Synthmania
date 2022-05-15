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
    TrackPartition readMidi(const char* path);

    std::vector<std::string> getMidiPorts();
    void openPort(uint port);
    void openPort(uint port, libremidi::midi_in::message_callback callback);

    ~MidiHandler();

   private:
    boost::lockfree::queue<Message> messages{100};
    uint64_t start_time;
    libremidi::midi_out out;
    libremidi::midi_in in;
};

uint64_t micros();