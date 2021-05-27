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

#ifndef __AUDIO_ENGINE_H__
#define __AUDIO_ENGINE_H__

#include <array>
#include <algorithm>
#include <variant>

#include "logging_macros.h"
#include "OboeApiType.h"
#include "DuplexEngine.h"
#include "decoder/WavDecoder.h"
#include "renderer/MicRenderer.h"

class AudioEngine {
public:
    AudioEngine();
    virtual ~AudioEngine();

    void setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst);
    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    bool setAudioApi(OboeApiType apiType);
    bool isAAudioRecommended();
    void setAAudioIfRecommended();
    bool setAudioFilePath(const char *filePath);
    void setBGMVolume(int volume);
    void setMicVolume(int volume);
    void setLoopBack(bool isActive);
    void seekPosition(long time);

    bool requestStart();
    bool requestStop();

    long getBGMCurrentTime();
    bool isBGMPlaying();

private:
    const char *TAG = "[AudioEngine]";

    DuplexEngine mDuplexEngine;
    WavDecoder mWavDecoder;
    MicRenderer mMicRenderer;

    static float calcVolume(int volume) {
        int vol;
        if (volume <= 0) {
            vol = 0;
        } else if (volume >= 100) {
            vol = 100;
        } else {
            vol = volume;
        }
        return float(vol / 100);
    }

};

#endif //__AUDIO_ENGINE_H__
