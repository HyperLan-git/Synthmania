#include "AudioSource.hpp"

AudioSource::AudioSource(bool destroyOnFinished) {
    int err;
    alGenSources(1, &sourceID);
    OPENAL_DEBUG("creating source(" << sourceID << ")");
    alSourcei(sourceID, AL_BUFFER, 0);
    OPENAL_DEBUG("filling source(" << sourceID << ")");
    destroy = destroyOnFinished;
}

AudioSource::AudioSource(AudioBuffer& data, bool destroyOnFinished)
    : AudioSource(destroyOnFinished) {
    setBuffer(data);
}

AudioSource::AudioSource(AudioSource&& other) : AudioSource(other.destroy) {
    *this = std::move(other);
}

AudioSource& AudioSource::operator=(AudioSource&& other) {
    std::swap(this->sourceID, other.sourceID);
    std::swap(this->destroy, other.destroy);
    return *this;
}

void AudioSource::setBuffer(AudioBuffer& data) {
    alSourcei(sourceID, AL_BUFFER, data.getBuffer());
    OPENAL_DEBUG("setting buffer");
}

void AudioSource::setDestroyOnFinished(bool destroy) {
    this->destroy = destroy;
}

void AudioSource::rewind() {
    alSourcePause(sourceID);
    OPENAL_DEBUG("pausing source(" << sourceID << ")");
    alSourceRewind(sourceID);
    OPENAL_DEBUG("rewinding source(" << sourceID << ")");
}

void AudioSource::play() {
    if (getState() != AL_PLAYING) alSourcePlay(sourceID);
    OPENAL_DEBUG("playing source(" << sourceID << ")");
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
        unsigned int id = buffers[i].getBuffer();
        alSourceQueueBuffers(sourceID, 1, &id);
    }
    OPENAL_DEBUG("queuing buffers");
}

void AudioSource::queueBuffer(AudioBuffer* buffer) {
    ALuint b = buffer->getBuffer();
    alSourceQueueBuffers(sourceID, 1, &b);
    OPENAL_DEBUG("queuing buffer");
}

ALuint* AudioSource::unqueueBuffers(int nb) {
    ALuint* buffers = new ALuint[nb];
    alSourceUnqueueBuffers(sourceID, nb, buffers);
    OPENAL_DEBUG("unqueuing buffers");
    return buffers;
}

ALfloat AudioSource::getSourcef(ALenum param) {
    ALfloat result;
    alGetSourcef(sourceID, param, &result);
    OPENAL_DEBUG("getting sourcef(" << param << ")");
    return result;
}

ALint AudioSource::getSourcei(ALenum param) {
    ALint result;
    alGetSourcei(sourceID, param, &result);
    OPENAL_DEBUG("getting sourcei(" << param << ")");
    return result;
}

glm::vec3 AudioSource::getSource3f(ALenum param) {
    glm::vec3 result;
    alGetSource3f(sourceID, param, &result.x, &result.y, &result.z);
    OPENAL_DEBUG("getting source3f(" << param << ")");
    return result;
}

void AudioSource::setSampleOffset(ALfloat value) {
    ALenum state = getState();
    rewind();
    setSourcef(AL_SAMPLE_OFFSET, value);
    if (state == AL_PLAYING) alSourcePlay(sourceID);
    OPENAL_DEBUG("playing source(" << sourceID << ")");
}

void AudioSource::setLooping(bool looping) { setSourcei(AL_LOOPING, looping); }

void AudioSource::setGain(ALfloat gain) { setSourcef(AL_GAIN, gain); }

void AudioSource::setPitch(ALfloat pitch) { setSourcef(AL_PITCH, pitch); }

void AudioSource::setSourcef(ALenum param, ALfloat value) {
    alSourcef(sourceID, param, value);
    OPENAL_DEBUG("setting sourcef(" << param << ")");
}

void AudioSource::setSource3f(ALenum param, glm::vec3 value) {
    alSource3f(sourceID, param, value.x, value.y, value.z);
    OPENAL_DEBUG("setting source3f(" << param << ")");
}

void AudioSource::setSourcei(ALenum param, ALint value) {
    alSourcei(sourceID, param, value);
    OPENAL_DEBUG("setting sourcei(" << param << ")");
}

AudioSource::~AudioSource() {
    alDeleteSources(1, &sourceID);
    OPENAL_DEBUG("deleting source(" << sourceID << ")");
}

bool AudioSource::operator==(const AudioSource& other) const {
    return this->sourceID == other.sourceID;
}