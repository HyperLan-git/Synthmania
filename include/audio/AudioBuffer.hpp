#pragma once

struct WAVData;
class AudioBuffer;

#include <AL/al.h>

#include <array>
#include <cstring>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>

class AudioBuffer {
   public:
    AudioBuffer();
    AudioBuffer(const char* file);

    void write(ALenum format, const ALvoid* data, ALsizei size,
               ALsizei samplerate);

    ALuint getBuffer();

    ALint getSamplerate();
    ALint getBits();
    ALint getChannels();
    ALint getSize();

    ~AudioBuffer();

   private:
    ALuint bufferID;
    ALint getBufferi(ALenum param);
};
