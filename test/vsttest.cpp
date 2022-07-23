#include <iostream>

class SimplePluginHost;
#include <thread>

#include "AudioHandler.hpp"
#include "SimplePluginHost.hpp"

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
        while (!(host->isVisible())) host->update();
        host->update();
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
        std::ofstream file("bin/synth.dat",
                           std::ofstream::out | std::ofstream::binary);
        host->savePluginData(file);
        file.close();
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
            std::cout << "gg\n";
            errored = false;
        } catch (std::exception e) {
            std::cerr << "error: " << e.what() << std::endl;
            std::cout << "error: " << e.what() << std::endl;
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

    std::thread thread(audiorun, new E{handler, &test}), thread2(guirun, &test);
    int result, result2;

    while (test.isActive()) std::this_thread::yield();

    std::cout << "t2\n";

    thread.join();
    thread2.join();

    return result | result2;
}