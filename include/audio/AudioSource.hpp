#pragma once

class AudioSource;

#include <AL/al.h>

#include <fstream>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "AudioBuffer.hpp"

class AudioSource {
   public:
    AudioSource(bool destroyOnFinished = true);
    AudioSource(AudioBuffer& data, bool destroyOnFinished = true);

    void setBuffer(AudioBuffer& data);

    void rewind();
    void play();

    ALenum getState();

    ALfloat getPitch();
    ALfloat getGain();
    ALfloat getMaxDistance();
    ALfloat getRolloffFactor();
    ALfloat getReferenceDistance();
    ALfloat getMinGain();
    ALfloat getMaxGain();
    ALfloat getConeOuterGain();
    ALfloat getConeInnerAngle();
    ALfloat getConeOuterAngle();
    ALfloat getSecOffset();
    ALfloat getSampleOffset();
    ALfloat getByteOffset();

    glm::vec3 getPosition();
    glm::vec3 getVelocity();
    glm::vec3 getDirection();

    bool isLooping();
    ALint getQueuedBuffers();
    ALint getProcessedBuffers();
    ALint getFreq();

    bool destroyOnFinished();

    void setDestroyOnFinished(bool destroy);

    void queueBuffers(AudioBuffer* buffers, int nb);
    ALuint* unqueueBuffers(int nb);

    void queueBuffer(AudioBuffer* buffer);

    void setSampleOffset(ALfloat value);
    void setLooping(bool looping);
    void setGain(ALfloat gain);

    ~AudioSource();

   private:
    ALfloat getSourcef(ALenum param);
    glm::vec3 getSource3f(ALenum param);
    ALint getSourcei(ALenum param);

    void setSourcef(ALenum param, ALfloat value);
    void setSource3f(ALenum param, glm::vec3 value);
    void setSourcei(ALenum param, ALint value);

    ALuint sourceID;
    bool destroy;
};