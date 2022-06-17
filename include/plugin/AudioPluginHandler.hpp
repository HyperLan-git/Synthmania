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

    void playNote(u_char pitch, u_char velocity, int64_t end);

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
    thrd_t synth_thread;  // :trollface:
    thrd_t gui_thread;

    std::map<u_char, int64_t> lingeringNotes;
};

int synthThread(void*);
int guiThread(void*);