#include <iostream>
#include <limits>
#include <thread>

#include "AudioHandler.hpp"
#include "AudioUtils.hpp"

#define BUFFERS 3
#define BUFFERSIZE 10024

#ifndef M_PI
#define M_PI 3.14159265359
#endif

int main() {
    std::vector<std::string> devices = getAudioDevices();
    for (auto device : devices) std::cout << device << std::endl;

    AudioHandler* handler = new AudioHandler();
    short buf[BUFFERSIZE] = {0};
    int sampleRate = handler->getSampleRate();
    AudioBuffer* buffers = new (BUFFERS) AudioBuffer[BUFFERS];
    AudioSource* source = new AudioSource(false);
    unsigned long t = 0;
    for (int i = 0; i < BUFFERS; i++) {
        for (int j = 0; j < BUFFERSIZE; j++)
            buf[j] = std::cos(2 * M_PI * (t++) / (double)sampleRate * (302.1)) *
                     32767;  // LEFT / RIGHT
        buffers[i].write(AL_FORMAT_STEREO16, buf, BUFFERSIZE * sizeof(short),
                         sampleRate);
    }
    source->queueBuffers(buffers, BUFFERS);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when queuing buffers:" << err << std::endl;
    source->play();
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when playing source:" << err << std::endl;
    handler->addSource(source);
    AudioBuffer* b;
    unsigned long i = 0;
    while (i++ < 500000) {  // hehe *burp*
        std::this_thread::yield();
        source->play();
        if ((err = alGetError()) != AL_NO_ERROR)
            std::cerr << "OpenAL error when playing source:" << err
                      << std::endl;
        int proc = source->getProcessedBuffers();
        if ((err = alGetError()) != AL_NO_ERROR)
            std::cerr << "OpenAL error when getting processed buffers:" << err
                      << std::endl;
        while (proc--) {
            ALuint* id = source->unqueueBuffers(1);
            if ((err = alGetError()) != AL_NO_ERROR)
                std::cerr << "OpenAL error when unqueuing buffers:" << err
                          << std::endl;
            for (int i = 0; i < BUFFERS; i++)
                if (buffers[i].getBuffer() == *id) {
                    b = buffers + i;
                    break;
                }
            for (int j = 0; j < BUFFERSIZE; j++) {
                buf[j] = std::cos(2 * M_PI * t / (double)sampleRate * (302.1)) *
                         32767;  // LEFT / RIGHT
                t++;
            }
            b->write(AL_FORMAT_STEREO16, buf, BUFFERSIZE * sizeof(short),
                     sampleRate);
            if ((err = alGetError()) != AL_NO_ERROR)
                std::cerr << "OpenAL error when writing to buffers:" << err
                          << std::endl;
            source->queueBuffer(b);
            if ((err = alGetError()) != AL_NO_ERROR)
                std::cerr << "OpenAL error when queuing buffers:" << err
                          << std::endl;
            delete[] id;
        }
    }
    for (int i = 0; i < BUFFERS; i++) buffers[i].setBuffer(0);
    delete[] buffers;
    AudioBuffer *buffer = new AudioBuffer("./resources/sounds/snare.wav"),
                *buffer2 = new AudioBuffer();
    std::cout << "sz : " << buffer->getSize()
              << "\nfreq : " << buffer->getSamplerate()
              << "\nformat : " << buffer->getChannels() << ","
              << buffer->getBits() << std::endl;
    short *d = std::get<short*>(buffer->getData()), *d2;
    short max = std::numeric_limits<short>().max();
    ALint ch = buffer->getChannels();
    ALsizei sz = buffer->getSize() * 8 / buffer->getBits() / ch, sz2 = 1;
    int samples = 2048, overlaps = 4;  // At least 2
    while (sz2 < sz) sz2 *= 2;
    sz = sz2;
    d2 = new short[sz * ch];
    for (int p = 0; p < sz / samples; p++) {
        for (int win = 0; win < overlaps; win++) {
            CArray data(samples);
            for (size_t i = 0; i < data.size(); i++) {
                data[i] = ((p * samples + i) >= sz / ch)
                              ? 0.0
                              : Complex(d[(p * samples + i) * ch]);
            }
            fft(data);
            // data = data.shift(1);
            //  data[0] += Complex(10000.0);
            double freq = buffer->getSamplerate() * 1.0 / samples;
            // data[1] = Complex(0, 100000000);
            ifft(data);
            for (size_t i = 0; i < data.size(); i++) {
                d2[(p * samples + i) * ch] = data[i].real();
                d2[(p * samples + i) * ch + 1] = 0;
            }
        }
    }
    buffer2->write(buffer->getFormat(), d2, sz * sizeof(short),
                   buffer->getSamplerate());
    delete[] d2;
    delete buffer;
    handler->addSound("test", buffer2);
    for (int i = 0; i < 10; i++) {
        AudioSource* src = handler->playSound("test");
        while (src->getState() != AL_STOPPED)
            ;
    }
    delete handler;
}