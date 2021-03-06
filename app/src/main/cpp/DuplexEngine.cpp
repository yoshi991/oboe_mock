/**
 * Copyright 2021 yoshi991
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "DuplexEngine.h"

DuplexEngine::~DuplexEngine() {
    closeStreams();
}

void DuplexEngine::setInputCallback(DuplexInputCallback *callback) {
    mInputCallback = callback;
}
void DuplexEngine::setOutputCallback(DuplexOutputCallback *callback) {
    mOutputCallback = callback;
}

void DuplexEngine::setSampleRate(int32_t sampleRate) {
    oboe::DefaultStreamValues::SampleRate = sampleRate;
}

void DuplexEngine::setFramesPerBurst(int32_t framesPerBurst) {
    oboe::DefaultStreamValues::FramesPerBurst = framesPerBurst;
}

void DuplexEngine::setRecordingDeviceId(int32_t deviceId) {
    mInputDeviceId = deviceId;
}

void DuplexEngine::setPlaybackDeviceId(int32_t deviceId) {
    mOutputDeviceId = deviceId;
}

bool DuplexEngine::isAAudioRecommended() {
    return oboe::AudioStreamBuilder::isAAudioRecommended();
}

int32_t DuplexEngine::getSampleRate() {
    return mSampleRate;
}

int32_t DuplexEngine::getChannelCount() {
    return mOutputChannelCount;
}

bool DuplexEngine::setAudioApi(OboeApiType apiType) {
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

void DuplexEngine::setPlaybackMicrophone(bool enabled) {
    mIsPlaybackMicrophone = enabled;
}

void DuplexEngine::setMute(bool isMute) {
    mIsMute = isMute;
}

bool DuplexEngine::openStreams() {
    // Note: The order of stream creation is important. We create the playback
    // stream first, then use properties from the playback stream
    // (e.g. sample rate) to create the recording stream. By matching the
    // properties we should get the lowest latency path
    oboe::Result result = openOutputStream();
    if (result != oboe::Result::OK) {
        return false;
    }

    result = openInputStream();
    if (result != oboe::Result::OK) {
        closeOutputStream();
    }
    return result == oboe::Result::OK;
}

bool DuplexEngine::closeStreams() {
    closeOutputStream();
    closeInputStream();
    return true;
}

bool DuplexEngine::start() {
    if (mIsPlaying) {
        LOGE("%s: streams has already started", TAG);
        return false;
    }

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

bool DuplexEngine::stop() {
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

oboe::AudioStreamBuilder DuplexEngine::createDefaultBuilder() {
    return *oboe::AudioStreamBuilder()
            .setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setAudioApi(mAudioApi)
            ->setFormat(mFormat);
}

oboe::AudioStreamBuilder DuplexEngine::createInputBuilder() {
    return *createDefaultBuilder().setDirection(oboe::Direction::Input)
            ->setDeviceId(mInputDeviceId)
            ->setSampleRate(mSampleRate)
            ->setChannelCount(mOutputStream->getChannelCount());
}

oboe::AudioStreamBuilder DuplexEngine::createOutputBuilder() {
    return *createDefaultBuilder().setDirection(oboe::Direction::Output)
            ->setCallback(this)
            ->setDeviceId(mOutputDeviceId)
            ->setChannelCount(mOutputChannelCount);
}

oboe::Result DuplexEngine::openInputStream() {
    oboe::Result result = createInputBuilder().openStream(mInputStream);
    if (result == oboe::Result::OK) {
        warnIfNotLowLatency(mInputStream);
    } else {
        LOGE("%s: input stream open failure: %s", TAG, oboe::convertToText(result));
    }
    return result;
}

oboe::Result DuplexEngine::openOutputStream() {
    oboe::Result result = createOutputBuilder().openStream(mOutputStream);
    if (result == oboe::Result::OK) {
        // The input stream needs to run at the same sample rate as the output.
        mSampleRate = mOutputStream->getSampleRate();
        warnIfNotLowLatency(mOutputStream);
    } else {
        LOGE("%s: output stream open failure: %s", TAG, oboe::convertToText(result));
        mSampleRate = oboe::kUnspecified;
    }
    return result;
}

/**
 * Warn in logcat if non-low latency stream is created
 * 
 * @param stream: newly created stream
 */
void DuplexEngine::warnIfNotLowLatency(std::shared_ptr<oboe::AudioStream> &stream) {
    if (stream->getPerformanceMode() != oboe::PerformanceMode::LowLatency) {
        LOGW("Stream is NOT low latency. Check your requested format, sample rate and channel count");
    }
}

void DuplexEngine::closeStream(std::shared_ptr<oboe::AudioStream> &stream) {
    if (!stream) return;

    oboe::Result result = stream->stop();
    if (result != oboe::Result::OK) {
        LOGW("%s: Error stopping stream: %s", TAG, oboe::convertToText(result));
    }
    result = stream->close();
    if (result != oboe::Result::OK) {
        LOGE("%s: Error closing stream: %s", TAG, oboe::convertToText(result));
    }
    stream.reset();
}

void DuplexEngine::closeInputStream() {
    closeStream(mInputStream);
}

void DuplexEngine::closeOutputStream() {
    closeStream(mOutputStream);
}

void DuplexEngine::setInputBufferSize() {
    // Determine maximum size that could possibly be called.
    int32_t bufferSize = mOutputStream->getBufferCapacityInFrames() * mOutputStream->getChannelCount();
    if (bufferSize > mBufferSize) {
        mInputBuffer = std::make_unique<float[]>(bufferSize);
        mBufferSize = bufferSize;
    }
}

oboe::DataCallbackResult DuplexEngine::onBothStreamsReady(
    std::shared_ptr<oboe::AudioStream> inputStream,
    void *inputData,
    int numInputFrames,
    std::shared_ptr<oboe::AudioStream> outputStream,
    void *outputData,
    int numOutputFrames
) {
    // This code assumes the data format for both streams is Float.
    int16_t *inputBuffer = static_cast<int16_t *>(inputData);
    int16_t *outputBuffer = static_cast<int16_t *>(outputData);

    // It also assumes the channel count for each stream is the same.
    int32_t samplesPerFrame = outputStream->getChannelCount();
    int32_t numInputSamples = numInputFrames * samplesPerFrame;
    int32_t numOutputSamples = numOutputFrames * samplesPerFrame;

    // It is possible that there may be fewer input than output samples.
    int32_t samplesToProcess = std::min(numInputSamples, numOutputSamples);
    if (mInputCallback) {
        mInputCallback->onInputReady(inputBuffer, inputStream->getChannelCount(), samplesToProcess);
    }
    if (mOutputCallback) {
        mOutputCallback->onOutputReady(outputBuffer, outputStream->getChannelCount(), samplesToProcess);
    }

    if (mIsPlaybackMicrophone) {
        for (int32_t i = 0; i < samplesToProcess; i++) {
            *outputBuffer++ += *inputBuffer++;
        }
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
oboe::DataCallbackResult DuplexEngine::onAudioReady(
    oboe::AudioStream *outputStream, 
    void *audioData, 
    int32_t numFrames
) {
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

    if (callbackResult == oboe::DataCallbackResult::Stop) {
        mInputStream->requestStop();
    }

    return callbackResult;
}

/**
 * Oboe notifies the application for "about to close the stream".
 *
 * @param oboeStream: the stream to close
 * @param result: oboe's reason for closing the stream
 */
void DuplexEngine::onErrorBeforeClose(
    oboe::AudioStream *oboeStream, 
    oboe::Result result
) {
    LOGE("%s: %s stream Error before close: %s",
        TAG,
        oboe::convertToText(oboeStream->getDirection()),
        oboe::convertToText(result)
    );

    mIsPlaying = false;
}

/**
 * Oboe notifies application that "the stream is closed"
 *
 * @param oboeStream
 * @param result
 */
void DuplexEngine::onErrorAfterClose(
    oboe::AudioStream *oboeStream, 
    oboe::Result result
) {
    LOGE("%s: %s stream Error after close: %s",
        TAG,
        oboe::convertToText(oboeStream->getDirection()),
        oboe::convertToText(result)
    );

    mIsPlaying = false;
    closeInputStream();
    if (result == oboe::Result::ErrorDisconnected) {
        openStreams();
        start();
    }
}
