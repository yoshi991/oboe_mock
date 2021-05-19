#ifndef __WAV_DECODER_H__
#define __WAV_DECODER_H__

#include <cstdlib>
#include <string>

#include "logging_macros.h"
#include "AudioCommon.h"

// parselib includes
#include "../libs/parselib/stream/MemInputStream.h"
#include "../libs/parselib/wav/WavStreamReader.h"

using namespace parselib;

class WavDecoder {
public:
    WavDecoder() {};
    virtual ~WavDecoder() = default;

    bool setAudioFilePath(const char *filePath);

    bool open(int32_t sampleRate, int32_t framesPerBurst);
    bool close();

    void render(float *buffer, int32_t channelCount, int32_t numFrames);

    bool isBGMPlaying();

private:
    bool mIsPlaying = false;
    float mOutputGain = 0.95;

    int32_t mSampleRate;

    const char *mAudioFilePath = nullptr;
    FILE *mAudioFile = nullptr;

    SampleFormat format;

    void analyzeWavInfo(int32_t sampleRate, int32_t framesPerBurst);

    static float shortToFloat(int16_t i) {
        float f;
        f = ((float) i) / (float) 32768;
        if (f > 1) f = 1;
        if (f < -1) f = -1;
        return f;
    };

};

#endif //__WAV_DECODER_H__