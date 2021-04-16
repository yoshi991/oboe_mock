#ifndef __AUDIO_ENGINE_H__
#define __AUDIO_ENGINE_H__

#include <array>
#include <algorithm>
#include <variant>

#include "logging_macros.h"
#include "OboeApiType.h"
#include "DuplexCallback.h"
#include "DuplexEngine.h"

class AudioEngine : DuplexCallback {
public:
    AudioEngine();
    virtual ~AudioEngine() = default;

    void setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst);
    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    bool setAudioApi(OboeApiType apiType);
    bool isAAudioRecommended(void);

    bool requestStart();
    bool requestStop();

    void onInputReady(const void *inputData, int32_t numFrames) override;
    void onOutputReady(void *outputData, int32_t numFrames) override;

private:
    const char *kTag = "[AudioEngine]";

    DuplexEngine mDuplexEngine;
};

#endif //__AUDIO_ENGINE_H__
