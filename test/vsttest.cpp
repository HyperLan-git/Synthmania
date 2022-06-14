#include <iostream>

class SimplePluginHost;
#include "AudioHandler.hpp"
#include "SimplePluginHost.hpp"
#include "threads.h"

#define BUFFER_LENGTH 10000
#define BUFFERS 5

struct E {
    AudioHandler* handler;
    SimplePluginHost* host;
};

int audiorun(void* arg) {
    try {
        SimplePluginHost* host = (SimplePluginHost*)arg;

        std::vector<std::string> devices = getDevices();
        // for (auto device : devices) std::cout << device << std::endl;
        short buf[BUFFER_LENGTH * 2] = {0};
        int sampleRate = handler->getSampleRate();
        AudioBuffer* buffers = new AudioBuffer[BUFFERS];
        AudioSource* source = new AudioSource();
        // std::cout << sampleRate << std::endl;
        for (int i = 0; i < BUFFERS; i++) {
            const float** channels = host->update();
            for (int j = 0; j < BUFFER_LENGTH * 2; j++)
                buf[j] = channels[j % 2][j] * 32760;  // LEFT / RIGHT
            buffers[i].write(AL_FORMAT_STEREO16, buf, BUFFER_LENGTH,
                             sampleRate);
        }
        source->queueBuffers(buffers, BUFFERS);
        handler->addSource(source);
        source->play();
        while (host->isVisible()) {
            // std::cout << std::hex << source->getState() << std::endl;
            source->play();
            int proc = source->getProcessedBuffers();
            while (proc--) {
                AudioBuffer* b = source->unqueueBuffers(1);
                const float** channels = host->update();
                for (int j = 0; j < BUFFER_LENGTH; j++) {
                    buf[j] = channels[j % 2][j] * 32760;  // LEFT / RIGHT
                }
                b->write(AL_FORMAT_STEREO16, buf, BUFFER_LENGTH, sampleRate);
                source->queueBuffer(b);
            }
        }
        host->stop();
        delete[] buffers;
        delete handler;
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

int guirun(void* arg) {
    int errored = false;
    SimplePluginHost* host = (SimplePluginHost*)arg;
    do {
        try {
            host->handleMessages();
            errored = false;
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
            errored = true;
        }
    } while (errored);
    return 0;
}

#include <stdexcept>
int main() {
    AudioHandler* handler = new AudioHandler();
    SimplePluginHost test("./plugins/Vital.vst3");

    thrd_t thread;
    int result;

    thrd_create(&thread, audiorun, &test);
    thrd_create(&thread, guirun, &test);

    while (handler)
        ;

    return result;
}