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
