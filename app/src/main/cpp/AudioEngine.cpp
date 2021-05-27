/**
 * Copyright 2021 yoshi991
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
