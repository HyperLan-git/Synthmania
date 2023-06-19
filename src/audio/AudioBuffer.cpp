#include "AudioBuffer.hpp"

AudioBuffer::AudioBuffer() : format(AL_NONE), data(NULL) {
    alGenBuffers(1, &bufferID);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when creating buffer:" << err << std::endl;
}

AudioBuffer::AudioBuffer(std::string file) : AudioBuffer() {
    try {
        AudioData* result = loadWavFile(file);
        this->write(result->format, result->data, result->size,
                    result->frequency);
        this->data = new char[result->size];
        this->format = result->format;
        memcpy(this->data, result->data, result->size);
        delete[] (unsigned char*)result->data;
        delete result;
    } catch (const char* err) {
        std::cerr << "Error when opening " << file << "\n" << err << std::endl;
        if (bufferID == AL_NONE)
            throw std::runtime_error("Could not read " + file +
                                     "\nCause : " + err);
    }
}

void AudioBuffer::write(ALenum format, const ALvoid* data, ALsizei size,
                        ALsizei samplerate) {
    alBufferData(bufferID, format, data, size, samplerate);
    // if (this->data) delete[] (unsigned char*)this->data;
    // this->data = new unsigned char[size];
    // memcpy(this->data, data, size);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when writing buffer:" << err << std::endl;
}

void* AudioBuffer::operator new[](size_t sz, size_t elems) noexcept {
    if (elems == sz) sz++;
    void* ptr = ::operator new[](sz);
    ALuint* p = (ALuint*)ptr;
    alGenBuffers(elems, p);
    p[elems] = 0;
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when creating buffers:" << err << std::endl;
    return ptr;
}

void AudioBuffer::operator delete[](void* ptr) noexcept {
    size_t sz = 0;
    for (ALuint* p = (ALuint*)ptr; *p != 0; p++) sz++;
    alDeleteBuffers(sz, (ALuint*)ptr);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffers:" << err << std::endl;
    ::operator delete[](ptr);
}

ALuint AudioBuffer::getBuffer() { return bufferID; }

ALint AudioBuffer::getSamplerate() { return getBufferi(AL_FREQUENCY); }

ALint AudioBuffer::getBits() { return getBufferi(AL_BITS); }

ALint AudioBuffer::getChannels() { return getBufferi(AL_CHANNELS); }

ALint AudioBuffer::getSize() { return getBufferi(AL_SIZE); }

std::variant<unsigned char*, short*> AudioBuffer::getData() {
    if (!data) throw std::runtime_error("getData null !");
    return (format == AL_FORMAT_MONO8 || format == AL_FORMAT_STEREO8)
               ? std::variant<unsigned char*, short*>((unsigned char*)data)
               : std::variant<unsigned char*, short*>((short*)data);
}

ALenum AudioBuffer::getFormat() { return format; }

ALint AudioBuffer::getBufferi(ALenum param) {
    ALint result;
    alGetBufferi(getBuffer(), param, &result);
    return result;
}

void AudioBuffer::setBuffer(ALuint id) {
    /*std::cout << "set:" << bufferID << (alIsBuffer(bufferID) ? 't' : 'f')
              << std::endl;
    if (bufferID != 0) alDeleteBuffers(1, &bufferID);
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffer:" << err <<
    std::endl;*/
    bufferID = id;
}

AudioBuffer::~AudioBuffer() {
    if (bufferID != 0 && alIsBuffer(bufferID)) alDeleteBuffers(1, &bufferID);
    if (data) delete[] (char*)data;
    int err;
    if ((err = alGetError()) != AL_NO_ERROR)
        std::cerr << "OpenAL error when deleting buffer:" << err << std::endl;
}