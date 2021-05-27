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

#ifndef __AUDIO_COMMON_H__
#define __AUDIO_COMMON_H__

struct SampleFormat {
    int32_t sampleRate;
    int32_t framesPerBuf;
    uint16_t channels;
    uint16_t pcmFormat;  // 8 bit, 16 bit, 24 bit ...
};

static float shortToFloat(int16_t i) {
    static constexpr float kSampleFullScale = (float) 0x8000;
    static constexpr float kInverseScale = 1.0f / kSampleFullScale;
    return (float) i * kInverseScale;
};

#endif //__AUDIO_COMMON_H__
