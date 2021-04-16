#ifndef __DUPLEX_ENGINE_H__
#define __DUPLEX_ENGINE_H__

#include <oboe/Oboe.h>

#include "logging_macros.h"
#include "DuplexCallback.h"
#include "OboeApiType.h"

class DuplexEngine : public oboe::AudioStreamCallback {
public:
    DuplexEngine() {};
    ~DuplexEngine();

    void setCallback(DuplexCallback *callback);

    void setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst);
    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    bool setAudioApi(OboeApiType apiType);
    bool isAAudioRecommended();

    void setPlaybackMicrophone(bool enabled);
    void setMute(bool isMute);

    bool openStreams();
    bool closeStreams();

    bool start();
    bool stop();

    /*
     * oboe::AudioStreamDataCallback interface implementation
     */
    oboe::DataCallbackResult onAudioReady(
        oboe::AudioStream *outputStream, 
        void *audioData, 
        int32_t numFrames
    ) override;

    /*
     * oboe::AudioStreamErrorCallback interface implementation
     */
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result result) override;
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result result) override;

private:
    const char *kTag = "[DuplexEngine]";

    DuplexCallback *mCallback = nullptr;

    bool mIsPlaying = false;
    bool mIsPlaybackMicrophone = false;
    bool mIsMute = false;

    int32_t mInputDeviceId = oboe::kUnspecified;
    int32_t mOutputDeviceId = oboe::kUnspecified;
    const oboe::AudioFormat mFormat = oboe::AudioFormat::Float; // for easier processing
    oboe::AudioApi mAudioApi = oboe::AudioApi::AAudio;
    int32_t mSampleRate = oboe::kUnspecified;
    const int32_t mInputChannelCount = oboe::ChannelCount::Stereo;
    const int32_t mOutputChannelCount = oboe::ChannelCount::Stereo;

    static constexpr int32_t kNumCallbacksToDrain = 20;
    static constexpr int32_t kNumCallbacksToDiscard = 30;
    // let input fill back up, usually 0 or 1
    int32_t mNumInputBurstsCushion = 1;

    // We want to reach a state where the input buffer is empty and
    // the output buffer is full.
    // These are used in order.
    // Drain several callback so that input is empty.
    int32_t mCountCallbacksToDrain = kNumCallbacksToDrain;
    // Let the input fill back up slightly so we don't run dry.
    int32_t mCountInputBurstsCushion = mNumInputBurstsCushion;
    // Discard some callbacks so the input and output reach equilibrium.
    int32_t mCountCallbacksToDiscard = kNumCallbacksToDiscard;

    std::shared_ptr<oboe::AudioStream> mInputStream;
    std::shared_ptr<oboe::AudioStream> mOutputStream;

    int32_t mBufferSize = 0;
    std::unique_ptr<float[]> mInputBuffer;

    oboe::AudioStreamBuilder createDefaultBuilder();
    oboe::AudioStreamBuilder createInputBuilder();
    oboe::AudioStreamBuilder createOutputBuilder();

    oboe::Result openInputStream();
    oboe::Result openOutputStream();
    void warnIfNotLowLatency(std::shared_ptr<oboe::AudioStream> &stream);

    void closeStream(std::shared_ptr<oboe::AudioStream> &stream);
    void closeInputStream();
    void closeOutputStream();

    void setInputBufferSize();

    oboe::DataCallbackResult onBothStreamsReady(
        std::shared_ptr<oboe::AudioStream> inputStream,
        const void *inputData,
        int   numInputFrames,
        std::shared_ptr<oboe::AudioStream> outputStream,
        void *outputData,
        int   numOutputFrames
    );

};

#endif //__DUPLEX_ENGINE_H__
