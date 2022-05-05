#include "AudioHandler.hpp"

AudioHandler::AudioHandler() {
    this->device = alcOpenDevice(nullptr);
    if (device == NULL) throw std::runtime_error("Couldn't open device !");
    ALCcontext* context = alcCreateContext(device, nullptr);
    if (context == NULL) throw std::runtime_error("Couldn't create context !");
    if (!alcMakeContextCurrent(context))
        throw std::runtime_error("Couldn't bind context !");
}