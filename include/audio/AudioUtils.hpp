#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <complex>
#include <fstream>
#include <valarray>
#include <variant>

#ifdef DEBUG
#define OPENAL_DEBUG(msg)                                                 \
    {                                                                     \
        int openal_err;                                                   \
        while ((openal_err = alGetError()) != AL_NO_ERROR)                \
            std::cerr << "OpenAL error when " << msg << ":" << openal_err \
                      << std::endl;                                       \
    }
#else
#define OPENAL_DEBUG(msg)
#endif

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

struct RIFF_Header {
    ALchar chunkID[4];
    ALsizei chunkSize;
    ALchar format[4];
};

struct WAVE_Format {
    ALchar subChunkID[4];
    ALsizei subChunkSize;
    ALshort audioFormat;
    ALshort numChannels;
    ALint sampleRate;
    ALint byteRate;
    ALshort blockAlign;
    ALshort bitsPerSample;
};

struct WAVE_Data {
    ALchar subChunkID[4];
    ALsizei subChunk2Size;
};

void fft(CArray& x);
void ifft(CArray& x);

unsigned int log2i(unsigned int n);

struct AudioData {
    ALsizei size, frequency;
    ALenum format;
    void* data;

    // Above 8 bits -> signed else unsigned
};

bool checkWavHeader(std::string filename);

AudioData loadWavFile(std::string filename);
