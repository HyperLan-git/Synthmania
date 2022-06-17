#include <iostream>
#include <thread>

#include "AudioHandler.hpp"

#define BUFFERS 30
#define BUFFERSIZE 1024

int main() {
    std::vector<std::string> devices = getDevices();
    for (auto device : devices) std::cout << device << std::endl;
    AudioHandler* handler = new AudioHandler();
    int sampleRate = handler->getSampleRate();
    short buf[BUFFERSIZE] = {0};
    AudioBuffer* buffers = new AudioBuffer[BUFFERS];
    AudioSource* source = new AudioSource();
    unsigned long t = 0;
    for (int i = 0; i < BUFFERS; i++) {
        for (int j = 0; j < BUFFERSIZE; j++) {
            buf[j] =
                32767 * std::cos(2 * M_PI * t / (double)sampleRate * (302.1));
            t++;
        }
        buffers[i].write(AL_FORMAT_MONO16, buf, BUFFERSIZE * sizeof(short),
                         sampleRate);
        // alBufferData(buffers[i].getBuffer(), AL_FORMAT_MONO16, buf,
        //                      BUFFER_LENGTH, sampleRate);
    }
    source->queueBuffers(buffers, BUFFERS);
    handler->addSource(source);
    source->play();
    unsigned long end = 0;
    while (true) {
        // std::cout << std::hex << source->getState() << std::endl;
        int proc = source->getProcessedBuffers();
        while (proc--) {
            AudioBuffer* b = source->unqueueBuffers(1);
            int bufNumber;
            for (bufNumber = 0; bufNumber < BUFFERS; bufNumber++)
                if (b->getBuffer() == buffers[bufNumber].getBuffer()) break;
            for (int i = 0; i < BUFFERSIZE; i++) {
                buf[i] = 32767 *
                         std::cos(2 * M_PI * t / (double)sampleRate * (302.1));
                t++;
            }
            b->write(AL_FORMAT_MONO16, buf, BUFFERSIZE * sizeof(short),
                     sampleRate);
            source->queueBuffer(b);
            source->play();
        }
    }
    delete[] buffers;
    delete handler;
}