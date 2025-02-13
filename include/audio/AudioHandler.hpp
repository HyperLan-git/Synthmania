#pragma once

class AudioHandler;

#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

#include "AudioBuffer.hpp"
#include "AudioSource.hpp"

std::vector<std::string> getAudioDevices();

// TODO lopass sidechain when playing kick
class AudioHandler {
   public:
    AudioHandler(const ALCchar* device = NULL);

    /**
     * @brief Will put a sound in the string map.
     *
     * @param name The name of the sound to be used in playSound
     * @param sound The buffer of the sound
     */
    AudioBuffer& addSound(std::string name, AudioBuffer&& sound);
    AudioSource& playSound(std::string name);

    void removeSound(AudioSource& source);

    void clearSounds();

    void setVolume(float volume);

    void setDevice(const ALCchar* device);

    AudioSource& addSource(AudioSource&& source);

    ALCint getSampleRate();

    bool update();

    ~AudioHandler();

   private:
    ALCdevice* device = NULL;
    int sampleRate;
    std::list<AudioSource> sources;
    std::unordered_map<std::string, AudioBuffer> sounds;
    float volume = 1.f;
};