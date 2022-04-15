#pragma once

class MidiHandler;
struct Message;

#define LIBREMIDI_HEADER_ONLY 1
#define LIBREMIDI_ALSA 1
#include <boost/lockfree/queue.hpp>
#include <chrono>
#include <condition_variable>
#include <libremidi/libremidi.hpp>
#include <mutex>
#include <queue>

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
    ~MidiHandler();

   private:
    boost::lockfree::queue<Message> messages{10};
    uint64_t start_time;
};

uint64_t micros();