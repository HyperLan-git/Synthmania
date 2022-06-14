#include <iostream>

#include "AudioHandler.hpp"

#define BUFFER_LENGTH 10000
#define BUFFERS 5

int main() {
    std::vector<std::string> devices = getDevices();
    for (auto device : devices) std::cout << device << std::endl;
    AudioHandler* handler = new AudioHandler();
    short buf[BUFFER_LENGTH] = {0};
    int sampleRate = handler->getSampleRate();
    AudioBuffer* buffers = new AudioBuffer[BUFFERS];
    AudioSource* source = new AudioSource();
    unsigned long t = 0;
    for (int i = 0; i < BUFFERS; i++) {
        for (int j = 0; j < BUFFER_LENGTH; j++) {
            buf[j] = std::cos(2 * M_PI * t / (double)sampleRate * 440.) * 32760;
            t++;
        }
        buffers[i].write(AL_FORMAT_MONO16, buf, BUFFER_LENGTH, sampleRate);
        // alBufferData(buffers[i].getBuffer(), AL_FORMAT_MONO16, buf,
        //                      BUFFER_LENGTH, sampleRate);
    }
    source->queueBuffers(buffers, BUFFERS);
    handler->addSource(source);
    source->play();
    while (true) {
        // std::cout << std::hex << source->getState() << std::endl;
        source->play();
        int proc = source->getProcessedBuffers();
        while (proc--) {
            AudioBuffer* b = source->unqueueBuffers(1);
            for (int i = 0; i < BUFFER_LENGTH; i++) {
                buf[i] =
                    std::cos(2 * M_PI * t / (double)sampleRate * 440.) * 32760;
                t++;
            }
            b->write(AL_FORMAT_MONO16, buf, BUFFER_LENGTH, sampleRate);
            source->queueBuffer(b);
            source->play();
        }
    }
    delete[] buffers;
    delete handler;
}