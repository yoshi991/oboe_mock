#ifndef __AUDIO_ENGINE_H__
#define __AUDIO_ENGINE_H__

#include <array>
#include <algorithm>
#include <variant>

#include "logging_macros.h"
#include "OboeApiType.h"
#include "DuplexCallback.h"
#include "DuplexEngine.h"
#include "decoder/WavDecoder.h"

// parselib includes
#include "libs/parselib/stream/MemInputStream.h"
#include "libs/parselib/wav/WavStreamReader.h"

#include "libs/iolib/player/OneShotSampleSource.h"

using namespace iolib;
using namespace parselib;

class AudioEngine : DuplexCallback {
public:
    AudioEngine();
    virtual ~AudioEngine();

    void setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst);
    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    bool setAudioApi(OboeApiType apiType);
    bool isAAudioRecommended(void);
    bool load(const char *filePath);
    bool loadSampleBuffer(unsigned char *buff, int32_t length);

    bool requestStart();
    bool requestStop();

    void onInputReady(float *inputFloats, int32_t channelCount, int32_t numFrames) override;
    void onOutputReady(float *outputFloats, int32_t channelCount, int32_t numFrames) override;

private:
    const char *TAG = "[AudioEngine]";

    float mInputGain = 0.95;
    float mOutputGain = 0.1;

    FILE *mAudioFile = nullptr;
    OneShotSampleSource *mBGMSource = nullptr;

    DuplexEngine mDuplexEngine;
    WavDecoder mWavDecoder;
};

#endif //__AUDIO_ENGINE_H__
