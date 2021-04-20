#ifndef __WAV_DECODER_H__
#define __WAV_DECODER_H__

#include <cstdlib>
#include <string>

#include "logging_macros.h"

class WavDecoder {
public:
    WavDecoder() {};
    virtual ~WavDecoder() = default;

    void load(const char *filePath);
    void play();
    void render(
        float *buffer, 
        int channel, 
        int32_t channelCount, 
        int32_t numFrames
    );

private:
    static const long MAX_FILE_SIZE = 30000000;

    int16_t *buffers;
    long size;
    int currentPositionL;
    int currentPositionR;

    bool isLoaded = false;

    long extractWav(const char *filePath);
    float shortToFloat(int16_t num);

};

#endif //__WAV_DECODER_H__