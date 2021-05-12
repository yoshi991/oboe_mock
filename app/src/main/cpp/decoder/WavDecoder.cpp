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
    float *buffer,
    int32_t channelCount,
    int32_t numFrames
) {
    if (isBGMPlaying()) {
        mBGMSource->mixAudio(buffer, channelCount, numFrames);
    }

    // if (mAudioFile == nullptr) return;

    // uint32_t samples = numFrames * channelCount;
    // int16_t *buffer = (int16_t *) calloc((size_t) samples, sizeof(int16_t));
    // unsigned char *buffer = (unsigned char *) calloc((size_t) mBufferSize, sizeof(unsigned char));
    // uint32_t size = fread(buffer, 1, samples, mAudioFile);
    // float *data = new float[samples];

    // getDataFloat(buffer, size, data);

    // int32_t samples = format.channels * format.sampleRate;
    // float *data = new float[samples];
    // getDataFloat(buffer, samples, data);

    // int32_t index = 0;
    // if (channelCount == 1) {
    //     // MONO output
    //     for (int32_t frameIndex = 0; frameIndex < numFrames; frameIndex++) {
    //         outputFloats[frameIndex] += buffer[index++];
    //     }
    // } else if (channelCount == 2) {
    //     // STEREO output
    //         int dstSampleIndex = 0;
    //     for (int32_t frameIndex = 0; frameIndex < numFrames; frameIndex++) {
    //         outputFloats[dstSampleIndex++] += buffer[index];
    //         outputFloats[dstSampleIndex++] += buffer[index++];
    //     }
    // }

    // delete[] buffer;
    // delete[] data;
}

bool WavDecoder::isBGMPlaying() {
    if (mBGMSource == nullptr) return false;
    return mBGMSource->isPlaying();
}

bool WavDecoder::isBGMPaused() {
    if (mBGMSource == nullptr) return false;
    return mBGMSource->isPaused();
}

void WavDecoder::setPauseMode() {
    mBGMSource->setPauseMode();
}

