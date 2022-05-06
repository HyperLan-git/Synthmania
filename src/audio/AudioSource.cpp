#include "AudioSource.hpp"

AudioSource::AudioSource(AudioBuffer data) {
    alGenSources(1, &sourceID);
    alSourcei(sourceID, AL_BUFFER, data.getBuffer());
}

void AudioSource::play() { alSourcePlay(sourceID); }

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

AudioSource::~AudioSource() { alDeleteSources(1, &sourceID); }