#pragma once

class AudioSource;

#include <AL/al.h>

#include <fstream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "AudioBuffer.hpp"

class AudioSource {
   public:
    AudioSource(AudioBuffer data);

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

    ~AudioSource();

   private:
    ALfloat getSourcef(ALenum param);
    glm::vec3 getSource3f(ALenum param);
    ALint getSourcei(ALenum param);

    ALuint sourceID;
};