/*
 * Copyright  2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "AudioEngine.h"
#include "logging_macros.h"

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

// oboe::Result AudioEngine::startStreams() {
//     oboe::Result result = outStream->requestStart();
//     int64_t timeoutNanos = 500 * 1000000; // arbitrary 1/2 second
//     auto currentState = outStream->getState();
//     auto nextState = oboe::StreamState::Unknown;
//     while (result == oboe::Result::OK && currentState != oboe::StreamState::Started) {
//         result = outStream->waitForStateChange(currentState, &nextState, timeoutNanos);
//         currentState = nextState;
//     }
//     if (result != oboe::Result::OK) return result;
//     return inStream->requestStart();
// }

// oboe::Result AudioEngine::stopStreams() {
//     oboe::Result outputResult = inStream->requestStop();
//     oboe::Result inputResult = outStream->requestStop();
//     if (outputResult != oboe::Result::OK) return outputResult;
//     return inputResult;
// }