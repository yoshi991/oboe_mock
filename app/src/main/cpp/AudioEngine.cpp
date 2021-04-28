#include "AudioEngine.h"

AudioEngine::AudioEngine() {
    mDuplexEngine.setCallback(this);
}

AudioEngine::~AudioEngine() {
    mDuplexEngine.setCallback(nullptr);
}

void AudioEngine::setDefaultStreamValues(int32_t sampleRate, int32_t framesPerBurst) {
    mDuplexEngine.setSampleRate(sampleRate);
    mDuplexEngine.setFramesPerBurst(framesPerBurst);
}

void AudioEngine::setRecordingDeviceId(int32_t deviceId) {
    mDuplexEngine.setRecordingDeviceId(deviceId);
}

void AudioEngine::setPlaybackDeviceId(int32_t deviceId) {
    mDuplexEngine.setPlaybackDeviceId(deviceId);
}

bool AudioEngine::isAAudioRecommended() {
    return mDuplexEngine.isAAudioRecommended();
}

bool AudioEngine::load(const char *filePath) {
    mAudioFile = nullptr;

    mAudioFile = fopen(filePath, "rb");
    if (mAudioFile == nullptr) {
        return false;
    }

    unsigned char *buffers = (unsigned char *) calloc((size_t) 30000000, sizeof(unsigned char));
    long size = fread(buffers, sizeof(short), 30000000, mAudioFile);

    fclose(mAudioFile);

    return loadSampleBuffer(buffers, size);
}

bool AudioEngine::loadSampleBuffer(unsigned char *buff, int32_t length) {
    mBGMSource = nullptr;

    MemInputStream stream(buff, length);
    WavStreamReader reader(&stream);
    reader.parse();

    SampleBuffer *sampleBuffer = new SampleBuffer();
    sampleBuffer->loadSampleData(&reader);

    mBGMSource = new OneShotSampleSource(sampleBuffer, mOutputGain);
    mBGMSource->setPlayMode();
    delete[] buff;

    return true;
}

bool AudioEngine::setAudioApi(OboeApiType apiType) {
    return mDuplexEngine.setAudioApi(apiType);
}

bool AudioEngine::requestStart() {
    bool success = mDuplexEngine.openStreams();
    if (success) {
        mDuplexEngine.start();
        mWavDecoder.play();
    }
    return success;
}

bool AudioEngine::requestStop() {
    mDuplexEngine.stop();
    mDuplexEngine.closeStreams();
    return true;
}

bool AudioEngine::isBGMPlaying() {
    if (mBGMSource == nullptr) return false;
    return mBGMSource->isPlaying();
}

void AudioEngine::onInputReady(
    float *inputFloats, 
    int32_t channelCount,
    int32_t numFrames
) {
    // TODO:
    for (int32_t i = 0; i < numFrames; i++) {
       *inputFloats++ *= mInputGain;
    }
}

void AudioEngine::onOutputReady(
    float *outputFloats, 
    int32_t channelCount,
    int32_t numFrames
) {
    // TODO:
    // for (int i = 0; i < channelCount; ++i) {
    //     // mWavDecoder.render(outputFloats, i, channelCount, numFrames);
    // }

    if (isBGMPlaying()) {
        mBGMSource->mixAudio(outputFloats, channelCount, numFrames);
    }

    // for (int32_t i = 0; i < numFrames; i++) {
    //    *outputFloats++ *= mOutputGain;
    // }
}