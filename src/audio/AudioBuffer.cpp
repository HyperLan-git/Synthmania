#include "AudioBuffer.hpp"

#include <AL/alut.h>

AudioBuffer::AudioBuffer() {
    alGenBuffers(1, &bufferID);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when creating buffer:" << err << std::endl;
}

AudioBuffer::AudioBuffer(ALuint id) { this->bufferID = id; }

AudioBuffer::AudioBuffer(std::string file) {
    bufferID = alutCreateBufferFromFile(file.c_str());
    if (bufferID == AL_NONE)
        throw std::runtime_error("Could not read " + file + "\nCause : " +
                                 alutGetErrorString(alutGetError()));
}

void AudioBuffer::write(ALenum format, const ALvoid* data, ALsizei size,
                        ALsizei samplerate) {
    alBufferData(bufferID, format, data, size, samplerate);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when writing buffer:" << err << std::endl;
}

ALuint AudioBuffer::getBuffer() { return bufferID; }

ALint AudioBuffer::getSamplerate() { return getBufferi(AL_FREQUENCY); }

ALint AudioBuffer::getBits() { return getBufferi(AL_BITS); }

ALint AudioBuffer::getChannels() { return getBufferi(AL_CHANNELS); }

ALint AudioBuffer::getSize() { return getBufferi(AL_SIZE); }

ALint AudioBuffer::getBufferi(ALenum param) {
    ALint result;
    alGetBufferi(getBuffer(), param, &result);
    return result;
}

void AudioBuffer::setBuffer(ALuint id) {
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffer:" << err << std::endl;
    bufferID = id;
}

AudioBuffer::~AudioBuffer() {
    if (bufferID != 0) alDeleteBuffers(1, &bufferID);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffer:" << err << std::endl;
}
