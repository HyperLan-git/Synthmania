#include "AudioHandler.hpp"

std::vector<std::string> getDevices() {
    std::vector<std::string> devices;

    // Récupération des devices disponibles
    const ALCchar* list = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

    if (list != NULL) {
        size_t l = strlen(list);
        while (l > 0) {
            devices.push_back(list);
            list += l + 1;
            l = strlen(list);
        }
    }
    return devices;
}

AudioHandler::AudioHandler(const ALCchar* device) {
    this->device = alcOpenDevice(device);
    if (this->device == NULL)
        throw std::runtime_error("Couldn't open device !");
    ALCcontext* context = alcCreateContext(this->device, nullptr);
    if (context == NULL) throw std::runtime_error("Couldn't create context !");
    if (!alcMakeContextCurrent(context))
        throw std::runtime_error("Couldn't bind context !");
    if (!alutInitWithoutContext(NULL, NULL))
        throw std::runtime_error("Couldn't init ALUT !");
    alcGetIntegerv(this->device, ALC_FREQUENCY, 1, &sampleRate);
}

AudioHandler::AudioHandler() : AudioHandler(NULL) {}

void AudioHandler::addSound(std::string name, AudioBuffer* sound) {
    sounds.emplace(name, sound);
}

AudioSource* AudioHandler::playSound(std::string name) {
    AudioSource* result = new AudioSource(*(sounds[name]));
    sources.push_back(result);
    result->play();
    return result;
}

ALCint AudioHandler::getSampleRate() { return sampleRate; }

bool AudioHandler::update() {
    for (auto iter = sources.begin(); iter != sources.end(); iter++) {
        ALenum state = (*iter)->getState();
        if (state == AL_STOPPED) {
            delete *iter;
            iter = sources.erase(iter);
            iter--;
        }
    }
    return !sources.empty();
}

AudioHandler::~AudioHandler() {
    for (AudioSource* source : sources) delete source;
    for (auto iter = sounds.begin(); iter != sounds.end(); iter++)
        delete (*iter).second;
    alutExit();
    ALCcontext* context = alcGetCurrentContext();
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}
