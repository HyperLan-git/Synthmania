#pragma once

#include <threads.h>

#include <iterator>
#include <vector>

#include "AudioHandler.hpp"
#include "MidiHandler.hpp"
#include "SimplePluginHost.hpp"

struct DelayedNoteOff {
    uint64_t timestamp;
    u_char pitch;
};

struct SynthParams {
    SimplePluginHost* host;
    AudioHandler* handler;
    size_t buffers;
    size_t bufferSize;
};

class AudioPluginHandler {
   public:
    AudioPluginHandler(std::string path, AudioHandler* handler);

    void noteOn(u_char pitch, u_char velocity);
    void noteOff(u_char pitch);

    void playNote(u_char pitch, u_char velocity, unsigned long end);

    void update(uint64_t time);

    SimplePluginHost* getHost();

    ~AudioPluginHandler();

   private:
    SimplePluginHost* host;
    thrd_t synth_thread;  // :trollface:
    thrd_t gui_thread;

    std::vector<DelayedNoteOff> lingeringNotes;
};

int synthThread(void*);
int guiThread(void*);