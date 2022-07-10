#pragma once

#ifdef _WIN32
#include "mingw.thread.h"
#else
#include <thread>
#endif

#include <iterator>
#include <vector>

#include "AudioHandler.hpp"
#include "MidiHandler.hpp"
#include "SimplePluginHost.hpp"

struct DelayedNoteOff {
    uint64_t timestamp;
    unsigned char pitch;
};

struct SynthParams {
    SimplePluginHost* host;
    AudioHandler* handler;
    size_t buffers;
    size_t bufferSize;
};

class AudioPluginHandler {
   public:
    AudioPluginHandler(std::string path, AudioHandler* handler,
                       std::string synthdata);

    void noteOn(unsigned char pitch, unsigned char velocity);
    void noteOff(unsigned char pitch);

    void playNote(unsigned char pitch, unsigned char velocity, int64_t end);

    void update(int64_t time);

    SimplePluginHost* getHost();

    ~AudioPluginHandler();

   private:
    /* TODO fix:
     if component methods are being called from threads other than the message
     thread, you'll need to use a MessageManagerLock object to make sure it's
     thread-safe (window destruction triggers this).
    */
    SimplePluginHost* host;
    std::thread synth_thread;  // :trollface:
    std::thread gui_thread;

    std::map<unsigned char, int64_t> lingeringNotes;
};

int synthThread(void*);
int guiThread(void*);