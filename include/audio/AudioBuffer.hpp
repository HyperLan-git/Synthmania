#pragma once

class AudioBuffer;

#include <AL/al.h>

#include <array>
#include <cstring>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>

#include "AudioUtils.hpp"

class AudioBuffer {
   public:
    AudioBuffer();
    AudioBuffer(std::string file);

    void* operator new[](size_t buffers) noexcept = delete;
    void* operator new[](size_t buffers, size_t elems) noexcept;
    void operator delete[](void* ptr) noexcept;

    void write(ALenum format, const ALvoid* data, ALsizei size,
               ALsizei samplerate);
    ALuint getBuffer();

    ALint getSamplerate();
    ALint getBits();
    ALint getChannels();
    ALint getSize();
    std::variant<unsigned char*, short*> getData();
    ALenum getFormat();

    void setBuffer(ALuint id);

    ~AudioBuffer();

   private:
    ALuint bufferID;
    void* data;
    ALenum format;
    ALint getBufferi(ALenum param);
};
