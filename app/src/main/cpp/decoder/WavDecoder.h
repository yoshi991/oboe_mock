#ifndef __WAV_DECODER_H__
#define __WAV_DECODER_H__

#include <cstdlib>
#include <string>

#include "logging_macros.h"
#include "AudioCommon.h"

// parselib includes
#include "../libs/parselib/stream/MemInputStream.h"
#include "../libs/parselib/wav/WavStreamReader.h"

#include "../libs/iolib/player/OneShotSampleSource.h"

using namespace iolib;
using namespace parselib;

class WavDecoder {
public:
    WavDecoder() {};
    virtual ~WavDecoder() = default;

    bool setAudioFilePath(const char *filePath);

    bool open();
    bool close();

    bool load(int32_t sampleRate, int32_t channelCount);
    bool loadSampleBuffer(unsigned char *buff, int32_t length);

    void render(float *buffer, int32_t channelCount, int32_t numFrames);

    bool isBGMPlaying();
    bool isBGMPaused();
    void setPauseMode();

private:
    static const long MAX_FILE_SIZE = 30000000;
    const uint16_t kBitPerSample = 16;

    bool mIsPlaying = false;
    float mOutputGain = 0.3;

    const char *mAudioFilePath = nullptr;
    FILE *mAudioFile = nullptr;

    unsigned char *mBGMBuffer;
    uint32_t mBufferSize;
    OneShotSampleSource *mBGMSource = nullptr;
    SampleFormat format;

    int32_t mCurFrameIndex;

    OneShotSampleSource* createSampleSource(unsigned char *buff, int32_t length);
    void getDataFloat(unsigned char *buff, int32_t length, float *data);

    static float shortToFloat(int16_t i) {
        float f;
        f = ((float) i) / (float) 32768;
        if (f > 1) f = 1;
        if (f < -1) f = -1;
        return f;
    };

};

#endif //__WAV_DECODER_H__