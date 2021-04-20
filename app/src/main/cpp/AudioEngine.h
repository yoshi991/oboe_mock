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

class AudioEngine : DuplexCallback {
public:
    AudioEngine();
    virtual ~AudioEngine() = default;

    void setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst);
    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    bool setAudioApi(OboeApiType apiType);
    bool isAAudioRecommended(void);
    void load(const char *filePath);

    bool requestStart();
    bool requestStop();

    void onInputReady(float *inputFloats, int32_t channelCount, int32_t numFrames) override;
    void onOutputReady(float *outputFloats, int32_t channelCount, int32_t numFrames) override;

private:
    const char *kTag = "[AudioEngine]";

    DuplexEngine mDuplexEngine;
    WavDecoder mWavDecoder;
};

#endif //__AUDIO_ENGINE_H__
