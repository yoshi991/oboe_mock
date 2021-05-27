#include "WavDecoder.h"

WavDecoder::WavDecoder() {
    mFormat.sampleRate = 0;
    mFormat.framesPerBuf = 0;
    mFormat.channels = 2;
    mFormat.pcmFormat = 16; // 16bit
}

bool WavDecoder::setAudioFilePath(const char *filePath) {
    mAudioFilePath = filePath;
    return true;
}

void WavDecoder::setSampleRate(int32_t sampleRate) {
    mFormat.sampleRate = sampleRate;
}

void WavDecoder::setFramesPerBurst(int32_t framesPerBurst) {
    mFormat.framesPerBuf = framesPerBurst;
}

bool WavDecoder::open() {
    mAudioFile = nullptr;

    mAudioFile = fopen(mAudioFilePath, "rb");
    if (mAudioFile == nullptr) return false;

    // 読み込み済みのbyte数を元に楽曲も途中から開始する
    fseek(mAudioFile, totalReadByte, SEEK_SET);

    return true;
}

bool WavDecoder::close() {
    if (mAudioFile == nullptr) return false;

    mIsPlaying = false;
    fclose(mAudioFile);
    mAudioFile = nullptr;
    return true;
}

void WavDecoder::start() {
    if (open()) {
        mIsPlaying = true;
    }
}

void WavDecoder::setVolume(float volume) {
    mOutputGain = volume;
}

void WavDecoder::seekPosition(long time) {
    totalReadByte = calculateBytePosition(time);
}

void WavDecoder::onOutputReady(
    int16_t *buffer,
    int32_t channelCount,
    int32_t numFrames
) {
    if (!mIsPlaying || numFrames == 0 || mAudioFile == nullptr) return;

    uint32_t samples = numFrames * channelCount;
    int16_t *bgmBuffer = (int16_t *) calloc((size_t) samples, sizeof(int16_t));
    memset(bgmBuffer, 0, sizeof(short) * samples);

    uint32_t size = fread(bgmBuffer, 1, samples, mAudioFile);
    if (size == 0) {
        mIsPlaying = false;
        delete[] bgmBuffer;
        return;
    }

    uint32_t numWriteFrames = std::min(static_cast<uint32_t>(numFrames), size);
    if (channelCount == 1) {
        for (int32_t frameIndex = 0; frameIndex < numWriteFrames / 2; frameIndex++) {
            int16_t left = int16_t(bgmBuffer[frameIndex] * mOutputGain);
            buffer[frameIndex] = left;
        }
    } else if (channelCount == 2) {
        int dstSampleIndex = 0;
        int currentIndex = 0;
        for (int32_t frameIndex = 0; frameIndex < numWriteFrames / 2; frameIndex++) {
            int16_t left = int16_t(bgmBuffer[currentIndex++] * mOutputGain);
            int16_t right = int16_t(bgmBuffer[currentIndex++] * mOutputGain);

            buffer[dstSampleIndex++] = left;
            buffer[dstSampleIndex++] = right;
        }
    }

    totalReadByte += static_cast<uint32_t>(size);
    delete[] bgmBuffer;
}

long WavDecoder::getBGMCurrentTime() {
    return (long) (1000 *
                   ((float) totalReadByte / (float) (mFormat.sampleRate * (mFormat.pcmFormat >> 3) * mFormat.channels)));
}

long WavDecoder::calculateBytePosition(long time) {
    return (long) (mFormat.sampleRate * (mFormat.pcmFormat >> 3) * mFormat.channels) * (time / 1000);
}

bool WavDecoder::isBGMPlaying() {
    return mIsPlaying;
}
