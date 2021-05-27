#ifndef __DUPLEX_CALLBACK_H__
#define __DUPLEX_CALLBACK_H__

class DuplexInputCallback {
public:
    virtual ~DuplexInputCallback() = default;

    virtual void onInputReady(
        int16_t *buffer, 
        int32_t channelCount,
        int32_t numFrames
    ) {}
};

class DuplexOutputCallback {
public:
    virtual ~DuplexOutputCallback() = default;

    virtual void onOutputReady(
        int16_t *buffer, 
        int32_t channelCount,
        int32_t numFrames
    ) {}
};

#endif //__DUPLEX_CALLBACK_H__