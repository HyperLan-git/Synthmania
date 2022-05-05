#include "AudioBuffer.hpp"

AudioBuffer::AudioBuffer() { alGenBuffers(1, &bufferID); }
