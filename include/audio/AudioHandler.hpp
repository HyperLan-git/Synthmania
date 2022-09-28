#pragma once

class AudioHandler;

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "AudioBuffer.hpp"
#include "AudioSource.hpp"

std::vector<std::string> getAudioDevices();

class AudioHandler {
   public:
    AudioHandler();
    AudioHandler(const ALCchar* device);

    void addSound(std::string name, AudioBuffer* sound);
    AudioSource* playSound(std::string name);

    void setDevice(const ALCchar* device);

    void addSource(AudioSource* source);

    ALCint getSampleRate();

    bool update();

    ~AudioHandler();

   private:
    ALCdevice* device = nullptr;
    int sampleRate;
    std::vector<AudioSource*> sources;
    std::map<std::string, AudioBuffer*> sounds;
};