#include <jni.h>

#include "logging_macros.h"
#include "OboeApiType.h"
#include "AudioEngine.h"

static AudioEngine *engine = nullptr;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_create(
    JNIEnv *env, jclass) {
    if (engine != nullptr) return JNI_TRUE;

    engine = new AudioEngine();
    return (engine != nullptr) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_delete(
    JNIEnv *env, jclass) {
    if (!engine) return;

    delete engine;
    engine = nullptr;
}

JNIEXPORT jboolean JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_requestStart(
    JNIEnv *env, jclass) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return JNI_FALSE;
    }

    engine->setLoopBack(true);
    return engine->requestStart() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_requestStop(
    JNIEnv *env, jclass) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return JNI_FALSE;
    }

    return engine->requestStop() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_setRecordingDeviceId(
    JNIEnv *env, jclass, jint deviceId) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return;
    }

    engine->setRecordingDeviceId(deviceId);
}

JNIEXPORT void JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_setPlaybackDeviceId(
    JNIEnv *env, jclass, jint deviceId) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return;
    }

    engine->setPlaybackDeviceId(deviceId);
}

/**
 * Set Audio API for Oboe.
 *
 * See: https://google.github.io/oboe/reference/classoboe_1_1_audio_stream_builder.html#a38c6d6c5e718df1e3ac69daaac47c391
 *
 * @param apiType 0: AAudio, 1: OpenSLES, else: Unspecified.
 */
JNIEXPORT jboolean JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_setAPI(
    JNIEnv *env, jclass type, jint apiType) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return JNI_FALSE;
    }

     OboeApiType oboeType;
     switch (apiType) {
         case static_cast<int>(OboeApiType::AAudio):
             oboeType = OboeApiType::AAudio;
             break;
         case static_cast<int>(OboeApiType::OpenSLES):
             oboeType = OboeApiType::OpenSLES;
             break;
         default:
             oboeType = OboeApiType::Unspecified;
             break;
     }

    return engine->setAudioApi(oboeType) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_isAAudioRecommended(
    JNIEnv *env, jclass type) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return JNI_FALSE;
    }
    return engine->isAAudioRecommended() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_loadFromAudioFile(
    JNIEnv *env, jclass type, jstring filePath
) {
    const char *pathUTF = env->GetStringUTFChars(filePath, 0);
    return engine->setAudioFilePath(pathUTF);
}

JNIEXPORT void JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_native_1setDefaultStreamValues(
    JNIEnv *env, jclass type, jint sampleRate, jint framesPerBurst) {
    engine->setDefaultStreamValues((int32_t) sampleRate, (int32_t) framesPerBurst);
}

JNIEXPORT jboolean JNICALL
Java_com_yoshi991_oboe_LiveEffectEngine_isBGMPlaying(
    JNIEnv *env, jclass type) {
    return engine->isBGMPlaying() ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"
