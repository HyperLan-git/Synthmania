#pragma once

class AudioBuffer;

#include <AL/al.h>

#include <iostream>
#include <string>

class AudioBuffer {
   public:
    AudioBuffer();

   private:
    ALuint bufferID;
};
