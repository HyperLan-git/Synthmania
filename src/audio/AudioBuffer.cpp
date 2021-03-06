#include "AudioBuffer.hpp"

#include <AL/alut.h>

AudioBuffer::AudioBuffer() { alGenBuffers(1, &bufferID); }

AudioBuffer::AudioBuffer(ALuint id) { this->bufferID = id; }

AudioBuffer::AudioBuffer(const char* file) {
    bufferID = alutCreateBufferFromFile(file);
}

void AudioBuffer::write(ALenum format, const ALvoid* data, ALsizei size,
                        ALsizei samplerate) {
    alBufferData(bufferID, format, data, size, samplerate);
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
    alDeleteBuffers(1, &bufferID);
    bufferID = id;
}

AudioBuffer::~AudioBuffer() { alDeleteBuffers(1, &bufferID); }
