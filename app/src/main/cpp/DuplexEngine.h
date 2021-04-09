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

#ifndef EFFECTS_APP_DUPLEXSTREAM_H
#define EFFECTS_APP_DUPLEXSTREAM_H

#include <array>
#include <algorithm>
#include <variant>
#include <oboe/Oboe.h>

#include "FunctionList.h"
#include "DuplexCallback.h"

class DuplexEngine {
public:
    DuplexEngine();
    virtual ~DuplexEngine() = default;

    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    bool setAudioApi(oboe::AudioApi);
    bool isAAudioRecommended(void);

    bool beginStreams();
    bool finishStreams();

    std::variant<FunctionList<int16_t *>, FunctionList<float *>> functionList{
            std::in_place_type<FunctionList<int16_t *>>};

private:
    bool mIsPlaying = false;
    int32_t mInputDeviceId = oboe::kUnspecified;
    int32_t mOutputDeviceId = oboe::kUnspecified;
    const oboe::AudioFormat mFormat = oboe::AudioFormat::Float; // for easier processing
    oboe::AudioApi mAudioApi = oboe::AudioApi::AAudio;
    int32_t mSampleRate = oboe::kUnspecified;
    const int32_t mInputChannelCount = oboe::ChannelCount::Stereo;
    const int32_t mOutputChannelCount = oboe::ChannelCount::Stereo;

    oboe::ManagedStream inStream;
    oboe::ManagedStream outStream;
    std::unique_ptr<oboe::AudioStreamCallback> mCallback;

    void openInStream();
    void openOutStream();

    oboe::Result startStreams();
    oboe::Result stopStreams();

    oboe::AudioStreamBuilder defaultBuilder();

    template<class numeric>
    void createCallback();

};


#endif //EFFECTS_APP_DUPLEXSTREAM_H
