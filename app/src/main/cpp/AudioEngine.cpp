#include "AudioEngine.h"

AudioEngine::AudioEngine() {
    mDuplexEngine.setCallback(this);
}

AudioEngine::~AudioEngine() {
    mDuplexEngine.setCallback(nullptr);
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

bool AudioEngine::setAudioFilePath(const char *filePath) {
    return mWavDecoder.setAudioFilePath(filePath);
}

bool AudioEngine::setAudioApi(OboeApiType apiType) {
    return mDuplexEngine.setAudioApi(apiType);
}

bool AudioEngine::requestStart() {
    bool success = mDuplexEngine.openStreams();
    if (success) {
        mWavDecoder.open(mDuplexEngine.getSampleRate(), mDuplexEngine.getChannelCount());
        mDuplexEngine.start();
    }
    return success;
}

bool AudioEngine::requestStop() {
    mWavDecoder.close();
    mDuplexEngine.stop();
    mDuplexEngine.closeStreams();
    return true;
}

bool AudioEngine::isBGMPlaying() {
    return mWavDecoder.isBGMPlaying();
}

void AudioEngine::onInputReady(
    float *inputFloats, 
    int32_t channelCount,
    int32_t numFrames
) {
    for (int32_t i = 0; i < numFrames; i++) {
       *inputFloats++ *= mInputGain;
    }
}

void AudioEngine::onOutputReady(
    float *outputFloats, 
    int32_t channelCount,
    int32_t numFrames
) {
    mWavDecoder.render(outputFloats, channelCount, numFrames);
}
