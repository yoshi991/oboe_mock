#include "AudioEngine.h"
#include "logging_macros.h"

void AudioEngine::setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst) {
    mDuplexCallback.setDefaultStreamValues(sampleRate, framesPerBurst);
}

void AudioEngine::setRecordingDeviceId(int32_t deviceId) {
    mDuplexCallback.setRecordingDeviceId(deviceId);
}

void AudioEngine::setPlaybackDeviceId(int32_t deviceId) {
    mDuplexCallback.setPlaybackDeviceId(deviceId);
}

bool AudioEngine::isAAudioRecommended() {
    return mDuplexCallback.isAAudioRecommended();
}

bool AudioEngine::setAudioApi(OboeApiType apiType) {
    return mDuplexCallback.setAudioApi(apiType);
}

bool AudioEngine::requestStart() {
    bool success = mDuplexCallback.openStreams();
    if (success) {
        mDuplexCallback.start();
    }
    return success;
}

bool AudioEngine::requestStop() {
    mDuplexCallback.stop();
    mDuplexCallback.closeStreams();
    return true;
}
