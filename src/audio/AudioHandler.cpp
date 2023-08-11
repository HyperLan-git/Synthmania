#include "AudioHandler.hpp"

std::vector<std::string> getAudioDevices() {
    std::vector<std::string> devices;

    // This list is just strings next to each other and then an empty string
    const ALCchar* list = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

    if (list != NULL) {
        size_t l = strlen(list);
        while (l > 0) {
            devices.push_back(std::string(list));
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
    alcGetIntegerv(this->device, ALC_FREQUENCY, 1, &sampleRate);
    OPENAL_DEBUG("creating context");
}

void AudioHandler::addSound(std::string name, AudioBuffer&& sound) {
    auto iter = sounds.find(name);
    if (iter != sounds.end()) sounds.erase(iter);
    auto elem = sounds.emplace(name, std::move(sound));
}

AudioSource& AudioHandler::playSound(std::string name) {
    AudioSource& result = sources.emplace_back(sounds[name]);
    result.setGain(volume);
    result.play();
    OPENAL_DEBUG("playing sound");
    return result;
}

void AudioHandler::addSource(AudioSource&& source) {
    sources.emplace_back(std::move(source));
}

ALCint AudioHandler::getSampleRate() { return sampleRate; }

void AudioHandler::setVolume(float volume) {
    this->volume = volume;
    // alListenerf(AL_GAIN, volume);
    for (AudioSource& source : this->sources) source.setGain(volume);
}

void AudioHandler::setDevice(const ALCchar* device) {
    clearSounds();
    ALCcontext* old = alcGetCurrentContext();
    alcDestroyContext(old);
    alcCloseDevice(this->device);
    this->device = alcOpenDevice(device);
    ALCcontext* context = alcCreateContext(this->device, nullptr);
    if (context == NULL) throw std::runtime_error("Couldn't create context !");
    if (!alcMakeContextCurrent(context))
        throw std::runtime_error("Couldn't bind context !");
}

bool AudioHandler::update() {
    for (auto iter = sources.begin(); iter != sources.end(); iter++) {
        if (!iter->destroyOnFinished()) continue;
        ALenum state = iter->getState();
        if (state == AL_STOPPED) {
            sources.erase(iter);
            return update();
        }
    }
    return !sources.empty();
}

void AudioHandler::removeSound(AudioSource& source) {
    for (auto iter = this->sources.begin(); iter != this->sources.end(); iter++)
        if (*iter == source) {
            this->sources.erase(iter);
            break;
        }
}

void AudioHandler::clearSounds() {
    sources.clear();
    sounds.clear();
}

AudioHandler::~AudioHandler() {
    int err;
    while ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error this session:" << err << "\n";
    clearSounds();
    ALCcontext* context = alcGetCurrentContext();
    alcCloseDevice(device);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
}
