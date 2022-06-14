#include "AudioPluginHandler.hpp"

AudioPluginHandler::AudioPluginHandler(std::string path,
                                       AudioHandler* handler) {
    this->host = new SimplePluginHost(path);

    thrd_create(&synth_thread, synthThread,
                new SynthParams{host, handler, 3, 1024});
    thrd_create(&gui_thread, guiThread, this);
}

void AudioPluginHandler::noteOn(u_char pitch, u_char velocity) {
    libremidi::message msg = libremidi::message::note_on(1, pitch, velocity);
    char* data = new char[msg.size()];
    memcpy(data, msg.bytes.data(), msg.size());
    host->addMidiMessage(SPH::MidiMessage{data, msg.size()});
}

void AudioPluginHandler::noteOff(u_char pitch) {
    libremidi::message msg = std::initializer_list<unsigned char>{
        libremidi::message::make_command(libremidi::message_type::NOTE_OFF, 1),
        pitch};
    char* data = new char[msg.size()];
    memcpy(data, msg.bytes.data(), msg.size());
    host->addMidiMessage(SPH::MidiMessage{data, msg.size()});
}

void AudioPluginHandler::playNote(u_char pitch, u_char velocity,
                                  unsigned long end) {
    noteOn(pitch, velocity);
    lingeringNotes.push_back({end, pitch});
}

void AudioPluginHandler::update(uint64_t time) {
    for (auto iter = lingeringNotes.begin(); iter != lingeringNotes.end();
         iter++) {
        if ((iter->timestamp) < (time)) {
            noteOff(iter->pitch);
            iter = lingeringNotes.erase(iter);
            iter--;
        }
    }
}

SimplePluginHost* AudioPluginHandler::getHost() { return host; }

AudioPluginHandler::~AudioPluginHandler() { delete host; }

int synthThread(void* arg) {
    try {
        SynthParams* params = (SynthParams*)arg;
        std::cout << params->buffers << std::endl;
        SimplePluginHost* host = params->host;

        AudioHandler* handler = params->handler;
        short buf[params->bufferSize * 2] = {0};
        int sampleRate = handler->getSampleRate();
        AudioBuffer* buffers = new AudioBuffer[params->buffers];
        AudioSource* source = new AudioSource(false);
        for (int i = 0; i < params->buffers; i++) {
            const float** channels = host->update();
            for (int j = 0; j < params->bufferSize * 2; j++)
                buf[j] = channels[j % 2][j] * 32760;  // LEFT / RIGHT
            buffers[i].write(AL_FORMAT_STEREO16, buf, params->bufferSize,
                             sampleRate);
        }
        source->queueBuffers(buffers, params->buffers);
        handler->addSource(source);
        source->play();
        while (true) {  // hehe *burp*
            source->play();
            int proc = source->getProcessedBuffers();
            while (proc--) {
                AudioBuffer* b = source->unqueueBuffers(1);
                const float** channels = host->update();
                for (int j = 0; j < params->bufferSize; j++) {
                    buf[j] = channels[j % 2][j] * 32760;  // LEFT / RIGHT
                }
                b->write(AL_FORMAT_STEREO16, buf, params->bufferSize,
                         sampleRate);
                source->queueBuffer(b);
            }
        }
        host->stop();
        delete params;
        delete[] buffers;
        delete source;
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