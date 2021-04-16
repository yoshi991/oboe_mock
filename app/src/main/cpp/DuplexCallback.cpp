#include "DuplexCallback.h"

DuplexCallback::~DuplexCallback() {
    closeStreams();
}

void DuplexCallback::setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst) {
    oboe::DefaultStreamValues::SampleRate = sampleRate;
    oboe::DefaultStreamValues::FramesPerBurst = framesPerBurst;
}

void DuplexCallback::setRecordingDeviceId(int32_t deviceId) {
    mInputDeviceId = deviceId;
}

void DuplexCallback::setPlaybackDeviceId(int32_t deviceId) {
    mOutputDeviceId = deviceId;
}

bool DuplexCallback::isAAudioRecommended() {
    return oboe::AudioStreamBuilder::isAAudioRecommended();
}

bool DuplexCallback::setAudioApi(OboeApiType apiType) {
    if (mIsPlaying) return false;

    oboe::AudioApi audioApi;
    switch (apiType) {
        case OboeApiType::AAudio:
            audioApi = oboe::AudioApi::AAudio;
            break;
        case OboeApiType::OpenSLES:
            audioApi = oboe::AudioApi::OpenSLES;
            break;
        default:
            audioApi = oboe::AudioApi::Unspecified;
            break;
    }

    mAudioApi = audioApi;
    return true;
}

void DuplexCallback::setPlaybackMicrophone(bool enabled) {
    mIsPlaybackMicrophone = enabled;
}

void DuplexCallback::setMute(bool isMute) {
    mIsMute = isMute;
}

bool DuplexCallback::openStreams() {
    // TODO:
    oboe::Result result = openInputStream();
    if (result != oboe::Result::OK) {
        return false;
    }
    return openOutputStream() == oboe::Result::OK;
}

bool DuplexCallback::closeStreams() {
    closeOutputStream();
    closeInputStream();
    return true;
}

bool DuplexCallback::start() {
    if (mIsPlaying) return false;

    mCountCallbacksToDrain = kNumCallbacksToDrain;
    mCountInputBurstsCushion = mNumInputBurstsCushion;
    mCountCallbacksToDiscard = kNumCallbacksToDiscard;

    setInputBufferSize();

    oboe::Result result = mInputStream->requestStart();
    if (result != oboe::Result::OK) {
        return false;
    }
    result = mOutputStream->requestStart();
    if (result != oboe::Result::OK) {
        return false;
    }
    mIsPlaying = true;
    return true;
}

bool DuplexCallback::stop() {
    if (!mIsPlaying) return false;

    oboe::Result outputResult = oboe::Result::OK;
    oboe::Result inputResult = oboe::Result::OK;
    
    if (mOutputStream) {
        outputResult = mOutputStream->requestStop();
    }
    if (mInputStream) {
        inputResult = mInputStream->requestStop();
    }
    mIsPlaying = false;

    if (outputResult != oboe::Result::OK) {
        return false;
    }
    return inputResult == oboe::Result::OK;
}

oboe::AudioStreamBuilder DuplexCallback::createDefaultBuilder() {
    return *oboe::AudioStreamBuilder()
            .setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setAudioApi(mAudioApi)
            ->setFormat(mFormat);
}

oboe::AudioStreamBuilder DuplexCallback::createInputBuilder() {
    return *createDefaultBuilder().setDirection(oboe::Direction::Input)
            ->setDeviceId(mInputDeviceId)
            ->setSampleRate(mSampleRate)
            ->setChannelCount(mInputChannelCount);
}

oboe::AudioStreamBuilder DuplexCallback::createOutputBuilder() {
    return *createDefaultBuilder().setDirection(oboe::Direction::Output)
            ->setDataCallback(this)
            ->setErrorCallback(this)
            ->setDeviceId(mOutputDeviceId)
            ->setChannelCount(mOutputChannelCount);
}

oboe::Result DuplexCallback::openInputStream() {
    oboe::Result result = createInputBuilder().openStream(mInputStream);
    if (result != oboe::Result::OK) {
        LOGE("%s: input stream open failure", kTag);
    }
    return result;
}

oboe::Result DuplexCallback::openOutputStream() {
    oboe::Result result = createOutputBuilder().openStream(mOutputStream);
    if (result != oboe::Result::OK) {
        LOGE("%s: output stream open failure", kTag);
    }
    return result;
}

void DuplexCallback::closeStream(std::shared_ptr<oboe::AudioStream> &stream) {
    if (!stream) return;

    oboe::Result result = stream->stop();
    if (result != oboe::Result::OK) {
        LOGW("%s: Error stopping stream: %s", kTag, oboe::convertToText(result));
    }
    result = stream->close();
    if (result != oboe::Result::OK) {
        LOGE("%s: Error closing stream: %s", kTag, oboe::convertToText(result));
    }
    stream.reset();
}

void DuplexCallback::closeInputStream() {
    closeStream(mInputStream);
}

void DuplexCallback::closeOutputStream() {
    closeStream(mOutputStream);
}

void DuplexCallback::setInputBufferSize() {
    // Determine maximum size that could possibly be called.
    int32_t bufferSize = mOutputStream->getBufferCapacityInFrames() * mOutputStream->getChannelCount();
    if (bufferSize > mBufferSize) {
        mInputBuffer = std::make_unique<float[]>(bufferSize);
        mBufferSize = bufferSize;
    }
}

oboe::DataCallbackResult DuplexCallback::onBothStreamsReady(
    std::shared_ptr<oboe::AudioStream> inputStream,
    const void *inputData,
    int   numInputFrames,
    std::shared_ptr<oboe::AudioStream> outputStream,
    void *outputData,
    int   numOutputFrames
) {
    // TODO:
    // Copy the input samples to the output with a little arbitrary gain change.

    if (mIsMute) {
        return oboe::DataCallbackResult::Continue;
    }

    // This code assumes the data format for both streams is Float.
    const float *inputFloats = static_cast<const float *>(inputData);
    float *outputFloats = static_cast<float *>(outputData);

    // It also assumes the channel count for each stream is the same.
    int32_t samplesPerFrame = outputStream->getChannelCount();
    int32_t numInputSamples = numInputFrames * samplesPerFrame;
    int32_t numOutputSamples = numOutputFrames * samplesPerFrame;

    // It is possible that there may be fewer input than output samples.
    int32_t samplesToProcess = std::min(numInputSamples, numOutputSamples);
    for (int32_t i = 0; i < samplesToProcess; i++) {
        *outputFloats++ = *inputFloats++ * 0.95; // do some arbitrary processing
    }

    // If there are fewer input samples then clear the rest of the buffer.
    int32_t samplesLeft = numOutputSamples - numInputSamples;
    for (int32_t i = 0; i < samplesLeft; i++) {
        *outputFloats++ = 0.0; // silence
    }

    return oboe::DataCallbackResult::Continue;
}

/**
 * Handles playback stream's audio request. In this sample, we simply block-read
 * from the record stream for the required samples.
 *
 * @param oboeStream: the playback stream that requesting additional samples
 * @param audioData:  the buffer to load audio samples for playback stream
 * @param numFrames:  number of frames to load to audioData buffer
 * @return: DataCallbackResult::Continue.
 */
oboe::DataCallbackResult DuplexCallback::onAudioReady(
    oboe::AudioStream *outputStream, 
    void *audioData, 
    int32_t numFrames
) {
    LOGD("[AudioPlayer]: DuplexCallback, onAudioReady");
        
    oboe::DataCallbackResult callbackResult = oboe::DataCallbackResult::Continue;
    int32_t actualFramesRead = 0;   
    // Silence the output.
    int32_t numBytes = numFrames * outputStream->getBytesPerFrame();
    memset(audioData, 0 /* value */, numBytes); 

    if (mCountCallbacksToDrain > 0) {
        // Drain the input.
        int32_t totalFramesRead = 0;
        do {
            oboe::ResultWithValue<int32_t> result = mInputStream->read(
                mInputBuffer.get(), numFrames, 0 /* timeout */);
            if (!result) {
                // Ignore errors because input stream may not be started yet.
                break;
            }
            actualFramesRead = result.value();
            totalFramesRead += actualFramesRead;
        } while (actualFramesRead > 0); 
        // Only counts if we actually got some data.
        if (totalFramesRead > 0) {
            mCountCallbacksToDrain--;
        }
    } else if (mCountInputBurstsCushion > 0) {
        // Let the input fill up a bit so we are not so close to the write pointer.
        mCountInputBurstsCushion--;
    } else if (mCountCallbacksToDiscard > 0) {
        // Ignore. Allow the input to reach to equilibrium with the output.
        oboe::ResultWithValue<int32_t> result = mInputStream->read(
            mInputBuffer.get(), numFrames, 0 /* timeout */);
        if (!result) {
            callbackResult = oboe::DataCallbackResult::Stop;
        }
        mCountCallbacksToDiscard--;
    } else {
        // Read data into input buffer.
        oboe::ResultWithValue<int32_t> result = mInputStream->read(
            mInputBuffer.get(), numFrames, 0 /* timeout */);
        if (!result) {
            callbackResult = oboe::DataCallbackResult::Stop;
        } else {
            int32_t framesRead = result.value();
            callbackResult = onBothStreamsReady(
                    mInputStream, mInputBuffer.get(), framesRead,
                    mOutputStream, audioData, numFrames
            );
        }
    }
    return callbackResult;
}

/**
 * Oboe notifies the application for "about to close the stream".
 *
 * @param oboeStream: the stream to close
 * @param error: oboe's reason for closing the stream
 */
void DuplexCallback::onErrorBeforeClose(
    oboe::AudioStream *oboeStream, 
    oboe::Result result
) {
    LOGE("%s stream Error before close: %s",
        oboe::convertToText(oboeStream->getDirection()),
        oboe::convertToText(result)
    );
}

/**
 * Oboe notifies application that "the stream is closed"
 *
 * @param oboeStream
 * @param error
 */
void DuplexCallback::onErrorAfterClose(
    oboe::AudioStream *oboeStream, 
    oboe::Result result
) {
    LOGE("%s stream Error after close: %s",
        oboe::convertToText(oboeStream->getDirection()),
        oboe::convertToText(result)
    );

    // inRef.close();
    // if (result == oboe::Result::ErrorDisconnected) {
    //     restart();
    // }
}
