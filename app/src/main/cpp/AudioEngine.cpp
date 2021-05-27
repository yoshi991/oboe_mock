#include "AudioEngine.h"

AudioEngine::AudioEngine() {
    mDuplexEngine.setInputCallback(&mMicRenderer);
    mDuplexEngine.setOutputCallback(&mWavDecoder);
}

AudioEngine::~AudioEngine() {
    mDuplexEngine.setInputCallback(nullptr);
    mDuplexEngine.setOutputCallback(nullptr);
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

bool AudioEngine::setAudioApi(OboeApiType apiType) {
    return mDuplexEngine.setAudioApi(apiType);
}

bool AudioEngine::isAAudioRecommended() {
    return mDuplexEngine.isAAudioRecommended();
}

void AudioEngine::setAAudioIfRecommended() {
    if (!isAAudioRecommended()) return;
    setAudioApi(OboeApiType::AAudio);
}

bool AudioEngine::setAudioFilePath(const char *filePath) {
    return mWavDecoder.setAudioFilePath(filePath);
}

void AudioEngine::setBGMVolume(int volume) {
    mWavDecoder.setVolume(calcVolume(volume));
}

void AudioEngine::setMicVolume(int volume) {
    mMicRenderer.setVolume(calcVolume(volume));
}

void AudioEngine::setLoopBack(bool isActive) {
    mDuplexEngine.setPlaybackMicrophone(isActive);
}

void AudioEngine::seekPosition(long time) {
    mWavDecoder.seekPosition(time);
}

bool AudioEngine::requestStart() {
    bool success = mDuplexEngine.openStreams();
    if (success) {
        mDuplexEngine.start();
        mWavDecoder.start();
    }
    return success;
}

bool AudioEngine::requestStop() {
    mWavDecoder.close();
    mDuplexEngine.stop();
    mDuplexEngine.closeStreams();
    return true;
}

long AudioEngine::getBGMCurrentTime() {
    return mWavDecoder.getBGMCurrentTime();
}

bool AudioEngine::isBGMPlaying() {
    return mWavDecoder.isBGMPlaying();
}
