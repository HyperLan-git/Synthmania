#include <iostream>

class SimplePluginHost;
#include "AudioHandler.hpp"
#include "SimplePluginHost.hpp"
#include "threads.h"

#define BUFFERS 4
#define BUFFERSIZE 700

// Rush E
struct E {
    AudioHandler* handler;
    SimplePluginHost* host;
};

int audiorun(void* arg) {
    try {
        E* e = (E*)arg;
        SimplePluginHost* host = e->host;
        auto* handler = e->handler;

        std::vector<std::string> devices = getDevices();
        // for (auto device : devices) std::cout << device << std::endl;
        short buf[BUFFERSIZE] = {0};
        int sampleRate = handler->getSampleRate();
        AudioBuffer* buffers = new AudioBuffer[BUFFERS];
        AudioSource* source = new AudioSource();
        // std::cout << sampleRate << std::endl;

        for (int i = 0; i < BUFFERS; i++) {
            const float** channels = host->update();
            for (int j = 0; j < BUFFERSIZE; j++)
                buf[j] = channels[j % 2][j] * 32767;  // LEFT / RIGHT
            buffers[i].write(AL_FORMAT_STEREO16, buf,
                             BUFFERSIZE * sizeof(short), sampleRate);
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
                for (int j = 0; j < BUFFERSIZE; j++) {
                    buf[j] = channels[j % 2][j] * 32767;  // LEFT / RIGHT
                }
                b->write(AL_FORMAT_STEREO16, buf, BUFFERSIZE * sizeof(short),
                         sampleRate);
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
    SimplePluginHost test("./plugins/Vital.vst3", handler->getSampleRate(),
                          BUFFERSIZE, true,
                          "./resources/songs/Aegleseeker/synth1.dat");

    thrd_t thread, thread2;
    int result, result2;

    thrd_create(&thread, audiorun, new E{handler, &test});
    thrd_create(&thread2, guirun, &test);

    while (test.isVisible())
        ;
    // Program never ends
    // Oh well do I even care lol
    thrd_join(thread, &result);
    thrd_join(thread, &result2);

    return result | result2;
}