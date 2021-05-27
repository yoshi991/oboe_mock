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

#ifndef __WAV_DECODER_H__
#define __WAV_DECODER_H__

#include <cstdlib>
#include <string>

#include "logging_macros.h"
#include "AudioCommon.h"
#include "DuplexCallback.h"

class WavDecoder : public DuplexOutputCallback {
public:
    WavDecoder();
    virtual ~WavDecoder() = default;

    bool setAudioFilePath(const char *filePath);
    void setSampleRate(int32_t sampleRate);
    void setFramesPerBurst(int32_t framesPerBurst);

    bool open();
    bool close();
    void start();

    void setVolume(float volume);
    void seekPosition(long time);

    void onOutputReady(int16_t *buffer, int32_t channelCount, int32_t numFrames) override;

    long getBGMCurrentTime();
    bool isBGMPlaying();

private:
    bool mIsPlaying = false;

    const char *mAudioFilePath = nullptr;
    FILE *mAudioFile = nullptr;

    SampleFormat mFormat;

    float mOutputGain = 0.5;
    long int mTotalReadByte = 0;

    long calculateBytePosition(long time);

};

#endif //__WAV_DECODER_H__