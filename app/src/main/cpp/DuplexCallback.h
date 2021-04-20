#ifndef __DUPLEX_CALLBACK_H__
#define __DUPLEX_CALLBACK_H__

class DuplexCallback {
public:
    virtual ~DuplexCallback() = default;

    virtual void onInputReady(float *inputFloats, int32_t numFrames) {}

    virtual void onOutputReady(float *outputFloats, int32_t numFrames) {}
};

#endif //__DUPLEX_CALLBACK_H__