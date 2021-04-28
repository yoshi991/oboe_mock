package com.yoshi991.oboe;

import android.content.Context;
import android.media.AudioManager;

public enum LiveEffectEngine {

    INSTANCE;

    // Load native library
    static {
        System.loadLibrary("liveEffect");
    }

    // Native methods
    static native boolean create();

    static native boolean isAAudioRecommended();

    static native boolean setAPI(int apiType);

    static native void loadFromAudioFile(String path);

    static native boolean loadFromByteArray(byte[] wavBytes);

    static native boolean requestStart();

    static native boolean requestStop();

    static native void setRecordingDeviceId(int deviceId);

    static native void setPlaybackDeviceId(int deviceId);

    static native void delete();

    static native void native_setDefaultStreamValues(int defaultSampleRate, int defaultFramesPerBurst);

    static void setDefaultStreamValues(Context context) {
        AudioManager myAudioMgr = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        String sampleRateStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        int defaultSampleRate = Integer.parseInt(sampleRateStr);
        String framesPerBurstStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        int defaultFramesPerBurst = Integer.parseInt(framesPerBurstStr);

        native_setDefaultStreamValues(defaultSampleRate, defaultFramesPerBurst);
    }
}