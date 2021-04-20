#include "AudioEngine.h"

AudioEngine::AudioEngine() {
    mDuplexEngine.setCallback(this);
}

void AudioEngine::setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst) {
    mDuplexEngine.setSampleRate(sampleRate);
    mDuplexEngine.setFramesPerBurst(framesPerBurst);
}

void AudioEngine::setRecordingDeviceId(int32_t deviceId) {
    mDuplexEngine.setRecordingDeviceId(deviceId);
}

void AudioEngine::setPlaybackDeviceId(int32_t deviceId) {
    mDuplexEngine.setPlaybackDeviceId(deviceId);
}

bool AudioEngine::isAAudioRecommended() {
    return mDuplexEngine.isAAudioRecommended();
}

void AudioEngine::load(const char *filePath) {
    mWavDecoder.load(filePath);
}

bool AudioEngine::setAudioApi(OboeApiType apiType) {
    return mDuplexEngine.setAudioApi(apiType);
}

bool AudioEngine::requestStart() {
    bool success = mDuplexEngine.openStreams();
    if (success) {
        mDuplexEngine.start();
    }
    return success;
}

bool AudioEngine::requestStop() {
    mDuplexEngine.stop();
    mDuplexEngine.closeStreams();
    return true;
}

void AudioEngine::onInputReady(float *inputFloats, int32_t numFrames) {
    // TODO:
    for (int32_t i = 0; i < numFrames; i++) {
       *inputFloats++ *= 0.95;
    }   
}

void AudioEngine::onOutputReady(float *outputFloats, int32_t numFrames) {
    // TODO:
}