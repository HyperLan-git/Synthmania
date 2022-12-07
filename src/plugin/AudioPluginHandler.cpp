#include "AudioPluginHandler.hpp"
#ifndef NOVST

AudioPluginHandler::AudioPluginHandler(std::string path, AudioHandler* handler,
                                       std::string synthdata) {
    this->host = new SimplePluginHost(path, handler->getSampleRate(), 1024,
                                      false, synthdata);

    synth_thread =
        std::thread(synthThread, new SynthParams{host, handler, 3, 1024});
    gui_thread = std::thread(guiThread, this);
}

void AudioPluginHandler::noteOn(unsigned char pitch, unsigned char velocity) {
    noteOff(pitch);
    libremidi::message msg = libremidi::message::note_on(1, pitch, velocity);
    char* data = new char[msg.size()];
    memcpy(data, msg.bytes.data(), msg.size());
    host->addMidiMessage(SPH::MidiMessage{data, msg.size()});
}

void AudioPluginHandler::noteOff(unsigned char pitch) {
    libremidi::message msg = std::initializer_list<unsigned char>{
        libremidi::message::make_command(libremidi::message_type::NOTE_OFF, 1),
        pitch, 0};
    char* data = new char[msg.size()];
    memcpy(data, msg.bytes.data(), msg.size());
    host->addMidiMessage(SPH::MidiMessage{data, msg.size()});
}

void AudioPluginHandler::playNote(unsigned char pitch, unsigned char velocity,
                                  int64_t end) {
    noteOn(pitch, velocity);
    if (lingeringNotes.count(pitch) == 0 || lingeringNotes[pitch] < end) {
        lingeringNotes.erase(pitch);
        lingeringNotes.emplace(pitch, end);
    }
}

void AudioPluginHandler::update(int64_t time) {
    for (auto iter = lingeringNotes.begin(); iter != lingeringNotes.end();
         iter++) {
        if ((iter->second) < (time)) {
            noteOff(iter->first);
            iter = lingeringNotes.erase(iter);
            if (iter == lingeringNotes.begin()) {
                update(time);
                return;
            }
            iter--;
        }
    }
}

SimplePluginHost* AudioPluginHandler::getHost() { return host; }

AudioPluginHandler::~AudioPluginHandler() {
    host->stop();
    gui_thread.join();
    synth_thread.join();
    delete host;
}

int synthThread(void* arg) {
    try {
        SynthParams* params = (SynthParams*)arg;
        SimplePluginHost* host = params->host;

        AudioHandler* handler = params->handler;
        short buf[params->bufferSize] = {0};
        int sampleRate = handler->getSampleRate();
        AudioBuffer* buffers =
            new (params->buffers) AudioBuffer[params->buffers];
        AudioSource* source = new AudioSource();
        for (int i = 0; i < params->buffers; i++) {
            const float** channels = host->update();
            for (int j = 0; j < params->bufferSize; j++)
                buf[j] = channels[j % 2][j] * 32767;  // LEFT / RIGHT
            buffers[i].write(AL_FORMAT_STEREO16, buf,
                             params->bufferSize * sizeof(short), sampleRate);
        }
        source->queueBuffers(buffers, params->buffers);
        int err;
        if ((err = alGetError()) != AL_NO_ERROR)
            std::cerr << "OpenAL error when queuing buffers:" << err
                      << std::endl;
        // handler->addSource(source);
        source->play();
        if ((err = alGetError()) != AL_NO_ERROR)
            std::cerr << "OpenAL error when playing source:" << err
                      << std::endl;
        AudioBuffer* b;
        while (host->isActive()) {  // hehe *burp*
            source->play();
            if ((err = alGetError()) != AL_NO_ERROR)
                std::cerr << "OpenAL error when playing source:" << err
                          << std::endl;
            int proc = source->getProcessedBuffers();
            if ((err = alGetError()) != AL_NO_ERROR)
                std::cerr << "OpenAL error when getting processed buffers:"
                          << err << std::endl;
            while (proc--) {
                ALuint* id = source->unqueueBuffers(1);
                if ((err = alGetError()) != AL_NO_ERROR)
                    std::cerr << "OpenAL error when unqueuing buffers:" << err
                              << std::endl;
                for (int i = 0; i < params->buffers; i++)
                    if (buffers[i].getBuffer() == *id) {
                        b = buffers + i;
                        break;
                    }
                const float** channels = host->update();
                if (channels == NULL) break;
                for (int j = 0; j < params->bufferSize; j++) {
                    buf[j] = channels[j % 2][j] * 32767;  // LEFT / RIGHT
                }
                b->write(AL_FORMAT_STEREO16, buf,
                         params->bufferSize * sizeof(short), sampleRate);
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
        for (int i = 0; i < params->buffers; i++) buffers[i].setBuffer(0);
        delete[] buffers;
        delete source;
        delete params;
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
int guiThread(void* arg) {
    int errored = false;
    do {
        try {
            AudioPluginHandler* handler = (AudioPluginHandler*)arg;
            handler->getHost()->handleMessages();
            errored = false;
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
            errored = true;
        }
    } while (errored);
    return 0;
}
#endif