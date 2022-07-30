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
    int err;
    while ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when creating:" << err << std::endl;
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

void AudioHandler::addSource(AudioSource* source) { sources.push_back(source); }

ALCint AudioHandler::getSampleRate() { return sampleRate; }

bool AudioHandler::update() {
    for (auto iter = sources.begin(); iter != sources.end(); iter++) {
        if (!(*iter)->destroyOnFinished()) continue;
        ALenum state = (*iter)->getState();
        if (state == AL_STOPPED) {
            delete *iter;
            sources.erase(iter);
            return update();
        }
    }
    return !sources.empty();
}

AudioHandler::~AudioHandler() {
    int err;
    while ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error this session:" << err << std::endl;
    for (AudioSource* source : sources) delete source;
    for (auto iter = sounds.begin(); iter != sounds.end(); iter++)
        delete (*iter).second;
    alutExit();
    ALCcontext* context = alcGetCurrentContext();
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}
