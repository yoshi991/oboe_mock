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

#include "MicRenderer.h"

void MicRenderer::setVolume(float volume) {
    mInputGain = volume;
}

void MicRenderer::onInputReady(
    int16_t *buffer, 
    int32_t channelCount, 
    int32_t numFrames
) {
    uint32_t numWriteFrames = static_cast<uint32_t>(numFrames);
    if (channelCount == 1) {
        for (int32_t frameIndex = 0; frameIndex < numWriteFrames / 2; frameIndex++) {
            buffer[frameIndex] *= mInputGain;;
        }
    } else if (channelCount == 2) {
        int dstSampleIndex = 0;
        for (int32_t frameIndex = 0; frameIndex < numWriteFrames / 2; frameIndex++) {
            buffer[dstSampleIndex++] *= mInputGain;
            buffer[dstSampleIndex++] *= mInputGain;
        }
    }
}
