#ifndef __MIC_RENDERER_H__
#define __MIC_RENDERER_H__

#include "logging_macros.h"
#include "DuplexCallback.h"

class MicRenderer : public DuplexInputCallback {
public:
    MicRenderer() {};
    virtual ~MicRenderer() = default;

    void setVolume(float volume);
    void onInputReady(int16_t *buffer, int32_t channelCount, int32_t numFrames) override;

private:
    float mInputGain = 0.5;

};

#endif //__MIC_RENDERER_H__