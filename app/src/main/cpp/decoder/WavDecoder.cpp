#include "WavDecoder.h"

bool WavDecoder::setAudioFilePath(const char *filePath) {
    mAudioFilePath = filePath;
    return true;
}

bool WavDecoder::open() {
    mAudioFile = nullptr;

    mAudioFile = fopen(mAudioFilePath, "rb");
    if (mAudioFile == nullptr) {
        return false;
    }
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

bool WavDecoder::load(int32_t sampleRate, int32_t channelCount) {
    open();

    mBufferSize = sampleRate * 1000 * channelCount * kBitPerSample;
    mBGMBuffer = (unsigned char *) calloc((size_t) mBufferSize, sizeof(unsigned char));
    long size = fread(mBGMBuffer, sizeof(short), mBufferSize, mAudioFile);

    close();
    return loadSampleBuffer(mBGMBuffer, size);
}

bool WavDecoder::loadSampleBuffer(unsigned char *buff, int32_t length) {
    mBGMSource = nullptr;
    mBGMSource = createSampleSource(buff, length);

    // format.channels = reader.getNumChannels();
    // format.framesPerBuf = framesPerBuf
    // format.pcmFormat = reader.getSampleEncoding();
    // format.sampleRate = reader.getSampleRate();

    return true;
}

OneShotSampleSource* WavDecoder::createSampleSource(unsigned char *buff, int32_t length) {
    MemInputStream stream(buff, length);
    WavStreamReader reader(&stream);
    reader.parse();

    SampleBuffer *sampleBuffer = new SampleBuffer();
    sampleBuffer->loadSampleData(&reader);

    OneShotSampleSource *sampleSource = new OneShotSampleSource(sampleBuffer, mOutputGain);
    sampleSource->setPlayMode();
    delete[] buff;
    return sampleSource;
}

void WavDecoder::getDataFloat(unsigned char *buff, int32_t length, float *data) {
    MemInputStream stream(buff, length);
    WavStreamReader reader(&stream);
    reader.parse();
    reader.getDataFloat(data, reader.getNumSampleFrames());
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
    uint32_t size = fread(buffer, sizeof(short), samples, mAudioFile);
    if (size == 0) {
        mIsPlaying = false;
        delete[] buffer;
        return;
    }

    if (channelCount == 1) {
        for (int32_t frameIndex = 0; frameIndex < numFrames; frameIndex++) {
            outputFloats[frameIndex] = shortToFloat(buffer[frameIndex]) * mOutputGain;
        }
    } else if (channelCount == 2) {
        int dstSampleIndex = 0;
        int currentIndex = 0;
        for (int32_t frameIndex = 0; frameIndex < numFrames; frameIndex++) {
            outputFloats[dstSampleIndex++] = shortToFloat(buffer[currentIndex]) * mOutputGain;
            outputFloats[dstSampleIndex++] = shortToFloat(buffer[currentIndex++]) * mOutputGain;
        }
    }
    delete[] buffer;
}

bool WavDecoder::isBGMPlaying() {
    return mIsPlaying;
}

bool WavDecoder::isBGMPaused() {
    if (mBGMSource == nullptr) return false;
    return mBGMSource->isPaused();
}

void WavDecoder::setPauseMode() {
    mBGMSource->setPauseMode();
}
