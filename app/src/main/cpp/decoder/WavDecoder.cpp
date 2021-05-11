#include "WavDecoder.h"

bool WavDecoder::setAudioFilePath(const char *filePath) {
    mAudioFilePath = filePath;
    return true;
}

bool WavDecoder::load(int32_t sampleRate, int32_t channelCount) {
    mAudioFile = nullptr;

    mAudioFile = fopen(mAudioFilePath, "rb");
    if (mAudioFile == nullptr) {
        return false;
    }

    mBufferSize = sampleRate * 1000 * channelCount * kBitPerSample;
    unsigned char *buffers = (unsigned char *) calloc((size_t) mBufferSize, sizeof(unsigned char));
    long size = fread(buffers, sizeof(short), mBufferSize, mAudioFile);

    fclose(mAudioFile);

    return loadSampleBuffer(buffers, size);
}


bool WavDecoder::loadSampleBuffer(unsigned char *buff, int32_t length) {
    mBGMSource = nullptr;

    MemInputStream stream(buff, length);
    WavStreamReader reader(&stream);
    reader.parse();

    SampleBuffer *sampleBuffer = new SampleBuffer();
    sampleBuffer->loadSampleData(&reader);

    mBGMSource = new OneShotSampleSource(sampleBuffer, mOutputGain);
    mBGMSource->setPlayMode();
    delete[] buff;

    format.channels = reader.getNumChannels();
    // format.framesPerBuf = framesPerBuf
    format.pcmFormat = reader.getSampleEncoding();
    format.sampleRate = reader.getSampleRate();

    return true;
}

void WavDecoder::render(
    float *buffer,
    int32_t channelCount,
    int32_t numFrames
) {
    // if (!isLoaded || currentPositionL < 0|| currentPositionR < 0) {
    //     memset(buffer, 0, sizeof(float) * numFrames);
    //     return;
    // }

    // if (channel == 0) {
    //     for (int i = 0, sampleIndex = 0; i < numFrames; i++) {
    //         buffer[sampleIndex] = shortToFloat(buffers[currentPositionL]);
    //         sampleIndex += channelCount;
    //         currentPositionL += channelCount;
    //     }
    //     if (currentPositionL >= size) {
    //         currentPositionL = -1;
    //     }
    // } else {
    //     for (int i = 0, sampleIndex = 0; i < numFrames; i++) {
    //         buffer[sampleIndex] = shortToFloat(buffers[currentPositionR]);
    //         sampleIndex += channelCount;
    //         currentPositionR += channelCount;
    //     }
    //     if (currentPositionR >= size) {
    //         currentPositionR = -1;
    //     }
    // }

    if (isBGMPlaying()) {
        mBGMSource->mixAudio(buffer, channelCount, numFrames);
    }
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

