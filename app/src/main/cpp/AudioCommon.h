#ifndef __AUDIO_COMMON_H__
#define __AUDIO_COMMON_H__

struct SampleFormat {
    int32_t sampleRate;
    int32_t framesPerBuf;
    uint16_t channels;
    uint16_t pcmFormat;  // 8 bit, 16 bit, 24 bit ...
};

static float shortToFloat(int16_t i) {
    static constexpr float kSampleFullScale = (float) 0x8000;
    static constexpr float kInverseScale = 1.0f / kSampleFullScale;
    return (float) i * kInverseScale;
};

#endif //__AUDIO_COMMON_H__
