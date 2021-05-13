#ifndef __AUDIO_COMMON_H__
#define __AUDIO_COMMON_H__

#include <variant>

/*
 * Audio Sample Controls...
 */
#define AUDIO_SAMPLE_CHANNELS 1

/*
 * Sample Buffer Controls...
 */
#define RECORD_DEVICE_KICKSTART_BUF_COUNT 2
#define PLAY_KICKSTART_BUFFER_COUNT 3
#define DEVICE_SHADOW_BUFFER_QUEUE_LEN 8
#define BUF_COUNT 32

struct SampleFormat {
    int32_t sampleRate;
    int32_t framesPerBuf;
    int32_t channels;
    int32_t pcmFormat;  // 8 bit, 16 bit, 24 bit ...
};

#endif //__AUDIO_COMMON_H__
