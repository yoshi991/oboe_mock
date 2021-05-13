#include "WavDecoder.h"

bool WavDecoder::setAudioFilePath(const char *filePath) {
    mAudioFilePath = filePath;
    return true;
}

bool WavDecoder::open(int32_t sampleRate, int32_t framesPerBurst) {
    mAudioFile = nullptr;

    mAudioFile = fopen(mAudioFilePath, "rb");
    if (mAudioFile == nullptr) {
        return false;
    }

    analyzeWavInfo(sampleRate, framesPerBurst);
    mIsPlaying = true;
    return true;
}

bool WavDecoder::close() {
    if (mAudioFile == nullptr) {
        return false;
    }
    fclose(mAudioFile);
    mAudioFile = nullptr;
    return true;
}

void WavDecoder::analyzeWavInfo(int32_t sampleRate, int32_t framesPerBurst) {
    if (mAudioFile == nullptr) return;

    uint32_t samples = sampleRate;
    unsigned char *buffer = (unsigned char *) calloc((size_t) samples, sizeof(unsigned char));
    uint32_t size = fread(buffer, sizeof(short), samples, mAudioFile);
    
    MemInputStream stream(buffer, size);
    delete[] buffer;

    WavStreamReader reader(&stream);
    reader.parse();

    format.sampleRate = sampleRate;
    format.framesPerBuf = framesPerBurst;
    format.channels = reader.getNumChannels();
    format.pcmFormat = reader.getSampleEncoding();
}

void WavDecoder::render(
    float *outputFloats,
    int32_t channelCount,
    int32_t numFrames
) {
    if (!mIsPlaying || numFrames == 0 || mAudioFile == nullptr) return;

    uint32_t samples = numFrames * channelCount;
    memset(outputFloats, 0, sizeof(float) * numFrames);

    int16_t *buffer = (int16_t *) calloc((size_t) samples, sizeof(int16_t));
    int32_t size = fread(buffer, sizeof(short), samples, mAudioFile);
    if (size == 0) {
        mIsPlaying = false;
        delete[] buffer;
        return;
    }

    int32_t numWriteFrames = std::min(numFrames, size);
    if (channelCount == 1) {
        for (int32_t frameIndex = 0; frameIndex < numWriteFrames; frameIndex++) {
            outputFloats[frameIndex] = shortToFloat(buffer[frameIndex]) * mOutputGain;
        }
    } else if (channelCount == 2) {
        int dstSampleIndex = 0;
        int currentIndex = 0;
        for (int32_t frameIndex = 0; frameIndex < numWriteFrames; frameIndex++) {
            outputFloats[dstSampleIndex++] = shortToFloat(buffer[currentIndex]) * mOutputGain;
            outputFloats[dstSampleIndex++] = shortToFloat(buffer[currentIndex++]) * mOutputGain;
        }
    }
    delete[] buffer;
}

bool WavDecoder::isBGMPlaying() {
    return mIsPlaying;
}
