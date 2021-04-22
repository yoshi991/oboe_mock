#include "WavDecoder.h"

void WavDecoder::load(const char *filePath) {
    LOGD("filePath = %s", filePath);

    buffers = (int16_t *) calloc((size_t) MAX_FILE_SIZE, sizeof(int16_t));
    size = extractWav(filePath);
    currentPositionL = -1;
    currentPositionR = -1;

    isLoaded = true;
}

void WavDecoder::play() {
    currentPositionL = 0;
    currentPositionR = 1;
}

void WavDecoder::render(
    float *buffer, 
    int channel, 
    int32_t channelCount,
    int32_t numFrames
) {
    // LOGD("channel = %1d, channelCount = %2d, numFrames = %3d", channel, channelCount, numFrames);

    if (!isLoaded || currentPositionL < 0|| currentPositionR < 0) {
        memset(buffer, 0, sizeof(float) * numFrames);
        return;
    }

    if (channel == 0) {
        for (int i = 0, sampleIndex = 0; i < numFrames; i++) {
            buffer[sampleIndex] = shortToFloat(buffers[currentPositionL]);
            sampleIndex += channelCount;
            currentPositionL += channelCount;
        }
        if (currentPositionL >= size) {
            currentPositionL = -1;
        }
    } else {
        for (int i = 0, sampleIndex = 0; i < numFrames; i++) {
            buffer[sampleIndex] = shortToFloat(buffers[currentPositionR]);
            sampleIndex += channelCount;
            currentPositionR += channelCount;
        }
        if (currentPositionR >= size) {
            currentPositionR = -1;
        }
    }
}

long WavDecoder::extractWav(const char *filePath) {
    FILE *fp = fopen(filePath, "rb");
    if (fp == nullptr) {
        return -1;
    }
    fseek(fp, 44, 0);
    long size = fread(buffers, sizeof(short), MAX_FILE_SIZE, fp);
    fclose(fp);
    return size;
}

float WavDecoder::shortToFloat(int16_t num) {
    float f;
    f = ((float) num) / (float) 32768;
    if (f > 1) f = 1;
    if (f < -1) f = -1;
    return f;
}
