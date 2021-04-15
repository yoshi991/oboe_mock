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

#include "OboeApiType.h"
#include "DuplexCallback.h"

class DuplexEngine {
public:
    DuplexEngine() {};
    virtual ~DuplexEngine() = default;

    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    bool setAudioApi(OboeApiType apiType);
    bool isAAudioRecommended(void);

    bool requestStart();
    bool requestStop();

private:
    DuplexCallback mDuplexCallback;

    // oboe::Result startStreams();
    // oboe::Result stopStreams();

};

#endif //EFFECTS_APP_DUPLEXSTREAM_H
