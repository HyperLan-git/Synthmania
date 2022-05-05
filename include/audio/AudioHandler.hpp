#pragma once
#include <AL/al.h>
#include <AL/alc.h>

#include <stdexcept>

class AudioHandler {
   public:
    AudioHandler();

   private:
    ALCdevice* device = nullptr;
};