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

bool AudioEngine::load() {
    return mWavDecoder.load(mDuplexEngine.getSampleRate(), mDuplexEngine.getChannelCount());
}

bool AudioEngine::loadSampleBuffer(unsigned char *buff, int32_t length) {
    return  mWavDecoder.loadSampleBuffer(buff, length);
}

bool AudioEngine::setAudioApi(OboeApiType apiType) {
    return mDuplexEngine.setAudioApi(apiType);
}

bool AudioEngine::requestStart() {
    bool success = mDuplexEngine.openStreams();
    if (success) {
        mDuplexEngine.start();
        load();
    }
    return success;
}

bool AudioEngine::requestStop() {
    mWavDecoder.setPauseMode();
    mDuplexEngine.stop();
    mDuplexEngine.closeStreams();
    return true;
}

bool AudioEngine::isBGMPlaying() {
    return mWavDecoder.isBGMPlaying();
}

bool AudioEngine::isBGMPaused() {
    return mWavDecoder.isBGMPaused();
}

void AudioEngine::onInputReady(
    float *inputFloats, 
    int32_t channelCount,
    int32_t numFrames
) {
    // TODO:
    for (int32_t i = 0; i < numFrames; i++) {
       *inputFloats++ *= mInputGain;
    }
}

void AudioEngine::onOutputReady(
    float *outputFloats, 
    int32_t channelCount,
    int32_t numFrames
) {
    // TODO:
    // for (int i = 0; i < channelCount; ++i) {
    //     // mWavDecoder.render(outputFloats, i, channelCount, numFrames);
    // }

    mWavDecoder.render(outputFloats, channelCount, numFrames);

    // for (int32_t i = 0; i < numFrames; i++) {
    //    *outputFloats++ *= mOutputGain;
    // }
}