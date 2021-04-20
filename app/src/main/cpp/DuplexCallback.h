#ifndef __DUPLEX_CALLBACK_H__
#define __DUPLEX_CALLBACK_H__

class DuplexCallback {
public:
    virtual ~DuplexCallback() = default;

    virtual void onInputReady(void *inputData, int32_t numFrames) {}

    virtual void onOutputReady(void *outputData, int32_t numFrames) {}
};

#endif //__DUPLEX_CALLBACK_H__