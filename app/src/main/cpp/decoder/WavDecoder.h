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
    long int totalReadByte = 0;

    long calculateBytePosition(long time);

};

#endif //__WAV_DECODER_H__