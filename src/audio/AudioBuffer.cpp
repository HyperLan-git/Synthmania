#include "AudioBuffer.hpp"

#include <AL/alut.h>

AudioBuffer::AudioBuffer() {
    alGenBuffers(1, &bufferID);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when creating buffer:" << err << std::endl;
}

AudioBuffer::AudioBuffer(std::string file) {
    bufferID = alutCreateBufferFromFile(file.c_str());
    if (bufferID == AL_NONE)
        throw std::runtime_error("Could not read " + file +
                                 "\nCause : " + std::to_string(alutGetError()));
}

void AudioBuffer::write(ALenum format, const ALvoid* data, ALsizei size,
                        ALsizei samplerate) {
    alBufferData(bufferID, format, data, size, samplerate);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when writing buffer:" << err << std::endl;
}

void* AudioBuffer::operator new[](size_t sz, size_t elems) noexcept {
    if (elems == sz) sz++;
    void* ptr = ::operator new[](sz);
    ALuint* p = (ALuint*)ptr;
    alGenBuffers(elems, p);
    p[elems] = 0;
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when creating buffers:" << err << std::endl;
    return ptr;
}

void AudioBuffer::operator delete[](void* ptr) noexcept {
    size_t sz = 0;
    for (ALuint* p = (ALuint*)ptr; *p != 0; p++) sz++;
    std::cout << "dels:" << *((ALuint*)ptr)
              << (alIsBuffer(*((ALuint*)ptr)) ? 't' : 'f') << std::endl;
    alDeleteBuffers(sz, (ALuint*)ptr);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffers:" << err << std::endl;
    ::operator delete[](ptr);
}

ALuint AudioBuffer::getBuffer() { return bufferID; }

ALint AudioBuffer::getSamplerate() { return getBufferi(AL_FREQUENCY); }

ALint AudioBuffer::getBits() { return getBufferi(AL_BITS); }

ALint AudioBuffer::getChannels() { return getBufferi(AL_CHANNELS); }

ALint AudioBuffer::getSize() { return getBufferi(AL_SIZE); }

ALint AudioBuffer::getBufferi(ALenum param) {
    ALint result;
    alGetBufferi(getBuffer(), param, &result);
    return result;
}

void AudioBuffer::setBuffer(ALuint id) {
    /*std::cout << "set:" << bufferID << (alIsBuffer(bufferID) ? 't' : 'f')
              << std::endl;
    if (bufferID != 0) alDeleteBuffers(1, &bufferID);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffer:" << err << std::endl;*/
    bufferID = id;
}

AudioBuffer::~AudioBuffer() {
    if (bufferID != 0) alDeleteBuffers(1, &bufferID);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffer:" << err << std::endl;
}
/* TODO will be useful for visualizer probably idk
#include <complex>
#include <iostream>
#include <valarray>

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
// Code "borrowed" from https://rosettacode.org/wiki/Fast_Fourier_transform#C++
void fft(CArray& x) {
    // DFT
    unsigned int N = x.size(), k = N, n;
    double thetaT = 3.14159265358979323846264338328L / N;
    Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
    while (k > 1) {
        n = k;
        k >>= 1;
        phiT = phiT * phiT;
        T = 1.0L;
        for (unsigned int l = 0; l < k; l++) {
            for (unsigned int a = l; a < N; a += n) {
                unsigned int b = a + k;
                Complex t = x[a] - x[b];
                x[a] += x[b];
                x[b] = t * T;
            }
            T *= phiT;
        }
    }
    // Decimate
    unsigned int m = (unsigned int)log2(N);
    for (unsigned int a = 0; a < N; a++) {
        unsigned int b = a;
        // Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a) {
            Complex t = x[a];
            x[a] = x[b];
            x[b] = t;
        }
    }
    //// Normalize (This section make it not working correctly)
    // Complex f = 1.0 / sqrt(N);
    // for (unsigned int i = 0; i < N; i++)
    //	x[i] *= f;
}
*/