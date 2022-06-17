#include "AudioSource.hpp"

AudioSource::AudioSource(bool destroyOnFinished) {
    alGenSources(1, &sourceID);
    // alSourcei(sourceID, AL_BUFFER, 0);
    destroy = destroyOnFinished;
}

AudioSource::AudioSource(AudioBuffer data, bool destroyOnFinished)
    : AudioSource(destroyOnFinished) {
    setBuffer(data);
}

void AudioSource::setBuffer(AudioBuffer data) {
    alSourcei(sourceID, AL_BUFFER, data.getBuffer());
}

void AudioSource::play() {
    if (getState() != AL_PLAYING) alSourcePlay(sourceID);
}

ALenum AudioSource::getState() { return getSourcei(AL_SOURCE_STATE); }

ALfloat AudioSource::getPitch() { return getSourcef(AL_PITCH); }
ALfloat AudioSource::getGain() { return getSourcef(AL_GAIN); }
ALfloat AudioSource::getMaxDistance() { return getSourcef(AL_MAX_DISTANCE); }
ALfloat AudioSource::getRolloffFactor() {
    return getSourcef(AL_ROLLOFF_FACTOR);
}
ALfloat AudioSource::getReferenceDistance() {
    return getSourcef(AL_REFERENCE_DISTANCE);
}
ALfloat AudioSource::getMinGain() { return getSourcef(AL_MIN_GAIN); }
ALfloat AudioSource::getMaxGain() { return getSourcef(AL_MAX_GAIN); }
ALfloat AudioSource::getConeOuterGain() {
    return getSourcef(AL_CONE_OUTER_GAIN);
}
ALfloat AudioSource::getConeInnerAngle() {
    return getSourcef(AL_CONE_INNER_ANGLE);
}
ALfloat AudioSource::getConeOuterAngle() {
    return getSourcef(AL_CONE_OUTER_ANGLE);
}
ALfloat AudioSource::getSecOffset() { return getSourcef(AL_SEC_OFFSET); }
ALfloat AudioSource::getSampleOffset() { return getSourcef(AL_SAMPLE_OFFSET); }
ALfloat AudioSource::getByteOffset() { return getSourcef(AL_BYTE_OFFSET); }

glm::vec3 AudioSource::getPosition() { return getSource3f(AL_POSITION); }
glm::vec3 AudioSource::getVelocity() { return getSource3f(AL_VELOCITY); }
glm::vec3 AudioSource::getDirection() { return getSource3f(AL_DIRECTION); }

bool AudioSource::isLooping() { return getSourcei(AL_LOOPING) == AL_TRUE; }
ALint AudioSource::getQueuedBuffers() { return getSourcei(AL_BUFFERS_QUEUED); }
ALint AudioSource::getProcessedBuffers() {
    return getSourcei(AL_BUFFERS_PROCESSED);
}
ALint AudioSource::getFreq() { return getSourcei(AL_FREQUENCY); }

bool AudioSource::destroyOnFinished() { return destroy; }

void AudioSource::queueBuffers(AudioBuffer* buffers, int nb) {
    for (int i = 0; i < nb; i++) {
        uint id = buffers[i].getBuffer();
        alSourceQueueBuffers(sourceID, 1, &id);
    }
}

void AudioSource::queueBuffer(AudioBuffer* buffer) {
    ALuint b = buffer->getBuffer();
    alSourceQueueBuffers(sourceID, 1, &b);
}

AudioBuffer* AudioSource::unqueueBuffers(int nb) {
    uint buffers[nb];
    alSourceUnqueueBuffers(sourceID, nb, buffers);
    AudioBuffer* ret = new AudioBuffer[nb];
    for (int i = 0; i < nb; i++) ret[i].setBuffer(buffers[i]);
    return ret;
}

ALfloat AudioSource::getSourcef(ALenum param) {
    ALfloat result;
    alGetSourcef(sourceID, param, &result);
    return result;
}

ALint AudioSource::getSourcei(ALenum param) {
    ALint result;
    alGetSourcei(sourceID, param, &result);
    return result;
}

glm::vec3 AudioSource::getSource3f(ALenum param) {
    glm::vec3 result;
    alGetSource3f(sourceID, param, &result.x, &result.y, &result.z);
    return result;
}

void AudioSource::setSampleOffset(ALfloat value) {
    setSourcef(AL_SAMPLE_OFFSET, value);
}

void AudioSource::setLooping(bool looping) { setSourcei(AL_LOOPING, looping); }

void AudioSource::setGain(ALfloat gain) { setSourcef(AL_GAIN, gain); }

void AudioSource::setSourcef(ALenum param, ALfloat value) {
    alSourcef(sourceID, param, value);
}

void AudioSource::setSource3f(ALenum param, glm::vec3 value) {
    alSource3f(sourceID, param, value.x, value.y, value.z);
}

void AudioSource::setSourcei(ALenum param, ALint value) {
    alSourcei(sourceID, param, value);
}

AudioSource::~AudioSource() { alDeleteSources(1, &sourceID); }