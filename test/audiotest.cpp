#include <iostream>
#include <thread>

#include "AudioHandler.hpp"

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
    delete handler;
}