#include "AudioUtils.hpp"

// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
// Code "borrowed" from https://rosettacode.org/wiki/Fast_Fourier_transform#C++
void fft(CArray& x) {
    // DFT
    unsigned int N = x.size(), k = N, n;
    double thetaT = 3.14159265358979323846264338328L / N;
    Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
    while (k > 1) {
        n = k;
        k >>= 1;
        phiT = phiT * phiT;
        T = 1.0L;
        for (unsigned int l = 0; l < k; l++) {
            for (unsigned int a = l; a < N; a += n) {
                unsigned int b = a + k;
                Complex t = x[a] - x[b];
                x[a] += x[b];
                x[b] = t * T;
            }
            T *= phiT;
        }
    }
    // Decimate
    unsigned int m = log2(N);
    for (unsigned int a = 0; a < N; a++) {
        unsigned int b = a;
        // Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a) {
            Complex t = x[a];
            x[a] = x[b];
            x[b] = t;
        }
    }
}

void ifft(CArray& x) {
    // conjugate the complex numbers
    x = x.apply(std::conj);

    // forward fft
    fft(x);

    // conjugate the complex numbers again
    x = x.apply(std::conj);

    // scale the numbers
    x /= x.size();
}

unsigned int log2i(unsigned int n) {
    unsigned int result = 0;
    while ((n = n << 1) > 0) result++;
    return result;
}

AudioData* loadWavFile(std::string filename) {
    FILE* soundFile = NULL;
    WAVE_Format wave_format;
    RIFF_Header riff_header;
    WAVE_Data wave_data;
    AudioData* result = new AudioData();

    try {
        soundFile = fopen(filename.c_str(), "rb");
        if (!soundFile) throw(filename);

        fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

        if ((riff_header.chunkID[0] != 'R' || riff_header.chunkID[1] != 'I' ||
             riff_header.chunkID[2] != 'F' || riff_header.chunkID[3] != 'F') &&
            (riff_header.format[0] != 'W' || riff_header.format[1] != 'A' ||
             riff_header.format[2] != 'V' || riff_header.format[3] != 'E'))
            throw("Invalid RIFF or WAVE Header");

        fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);

        if (wave_format.subChunkID[0] != 'f' ||
            wave_format.subChunkID[1] != 'm' ||
            wave_format.subChunkID[2] != 't' ||
            wave_format.subChunkID[3] != ' ')
            throw("Invalid Wave Format");

        if (wave_format.subChunkSize > 16)
            fseek(soundFile, sizeof(short), SEEK_CUR);

        fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);

        if (wave_data.subChunkID[0] != 'd' || wave_data.subChunkID[1] != 'a' ||
            wave_data.subChunkID[2] != 't' || wave_data.subChunkID[3] != 'a')
            throw("Invalid data header");

        result->data = new unsigned char[wave_data.subChunk2Size];

        if (!fread(result->data, wave_data.subChunk2Size, 1, soundFile))
            throw("error loading WAVE data into struct!");

        result->size = wave_data.subChunk2Size;
        result->frequency = wave_format.sampleRate;

        if (wave_format.numChannels == 1) {
            if (wave_format.bitsPerSample == 8)
                result->format = AL_FORMAT_MONO8;
            else if (wave_format.bitsPerSample == 16)
                result->format = AL_FORMAT_MONO16;
        } else if (wave_format.numChannels == 2) {
            if (wave_format.bitsPerSample == 8)
                result->format = AL_FORMAT_STEREO8;
            else if (wave_format.bitsPerSample == 16)
                result->format = AL_FORMAT_STEREO16;
        }
        fclose(soundFile);
    } catch (const char* error) {
        if (soundFile) fclose(soundFile);
        delete result;
        throw error;
    }
    return result;
}