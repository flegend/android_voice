/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* This is a JNI example where we use native methods to play sounds
 * using OpenSL ES. See the corresponding Java source file located at:
 *
 *   src/com/example/nativeaudio/NativeAudio/NativeAudio.java
 */

#include <stdlib.h>
#include <jni.h>
#include <string.h>
#include <pthread.h>


// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
// #include <android/log.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>

#include "OpenSLESCore.h"

#include "native_debug.h"
#include "../include/my_voice_engine_interface.h"

// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;
static SLMuteSoloItf bqPlayerMuteSolo;
static SLVolumeItf bqPlayerVolume;
static SLmilliHertz bqPlayerSampleRate = 0;
static jint bqPlayerBufSize = 0;
static short *resampleBuf = NULL;
// a mutext to guard against re-entrance to record & playback
// as well as make recording and playing back to be mutually exclusive
// this is to avoid crash at situations like:
//    recording is in session [not finished]
//    user presses record button and another recording coming in
// The action: when recording/playing back is not finished, ignore the new request
static pthread_mutex_t audioEngineLock = PTHREAD_MUTEX_INITIALIZER;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// URI player interfaces
static SLObjectItf uriPlayerObject = NULL;
static SLPlayItf uriPlayerPlay;
static SLSeekItf uriPlayerSeek;
static SLMuteSoloItf uriPlayerMuteSolo;
static SLVolumeItf uriPlayerVolume;

// file descriptor player interfaces
static SLObjectItf fdPlayerObject = NULL;
static SLPlayItf fdPlayerPlay;
static SLSeekItf fdPlayerSeek;
static SLMuteSoloItf fdPlayerMuteSolo;
static SLVolumeItf fdPlayerVolume;

// recorder interfaces
static SLObjectItf recorderObject = NULL;
static SLRecordItf recorderRecord;
static SLAndroidSimpleBufferQueueItf recorderBufferQueue;

// synthesized sawtooth clip
#define SAWTOOTH_FRAMES 8000
static short sawtoothBuffer[SAWTOOTH_FRAMES];

// 5 seconds of recorded audio at 16 kHz mono, 16-bit signed little endian
#define RECORDER_FRAMES (16000 * 5)
static short recorderBuffer[RECORDER_FRAMES];
static unsigned recorderSize = 0;

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

VOICEENGINEAPI::CAudioRecord *g_pRecord = NULL;
VOICEENGINEAPI::CAudioPlayer *g_pPlayer = NULL;
VOICEENGINEAPI::CAudioPlayer *g_pPlayer1 = NULL;
bool g_bUseOpenCore = true;

FILE *pcmFile = NULL;

FILE *pcmFileRead = NULL;

char* buf = NULL;
#define FILE_PATH "/sdcard/zsy.pcm"

class RecordData : public Transport {
public:
    RecordData(){
        pcmFile = fopen(FILE_PATH, "w");
        if (!pcmFile) {
            LOGE("fopen /sdcard/zsy.pcm error");
        }
    }
    ~RecordData() {
        if (pcmFile) {
            fclose(pcmFile);
            pcmFile = NULL;
        }
    }

    int SendPacket(int channel, const void *data, size_t len) {
        if (pcmFile) {
            LOGD("RecordData SendPacket len = %d", len);
            fwrite(data, 1, len, pcmFile);
        }
        return 0;
    }
    int SendRTCPPacket(int channel, const void *data, size_t len) {
        return 0;
    }
};

void ProduceData() {
    if (NULL == buf) {
        buf = new char[80];
    }

    if (NULL == pcmFileRead) {
        pcmFileRead = fopen(FILE_PATH, "rb");
    }
    if (pcmFileRead) {
        while (feof(pcmFileRead) == 0) {
            int len = fread(buf, 1, 92, pcmFileRead);
            if (len > 0) {
                LOGD("ProduceData len = %d", len);
                MyVoiceEngineInterface::GetVoiceEngine()->Decode(0, buf, len);
            }
        }
    }


    if (pcmFileRead) {
        fclose(pcmFileRead);
        pcmFileRead = NULL;
        LOGD("zsy fclose(pcmFileRead)");
    }

    if (buf) {
        free(buf);
        buf = NULL;
        LOGD("zsy free(buf)");
    }
}

RecordData* g_pRecordData = NULL;

// create the engine and output mix objects
extern "C" JNIEXPORT void JNICALL
Java_com_example_nativeaudio_NativeAudio_createEngine(JNIEnv *env, jclass clazz) {
    if (g_bUseOpenCore) {

        MyVoiceEngineInterface::GetVoiceEngine()->Init();

//        VOICEENGINEAPI::CAudioEngine::Ins().CreateEngine();
//        g_pRecord = new VOICEENGINEAPI::CAudioRecord(VOICEENGINEAPI::CAudioEngine::Ins().GetEngine());
//        g_pPlayer = new VOICEENGINEAPI::CAudioPlayer(VOICEENGINEAPI::CAudioEngine::Ins().GetEngine(), VOICEENGINEAPI::CAudioEngine::Ins().GetOutputMixObject(), 0);
//        g_pPlayer1 = new VOICEENGINEAPI::CAudioPlayer(VOICEENGINEAPI::CAudioEngine::Ins().GetEngine(), VOICEENGINEAPI::CAudioEngine::Ins().GetOutputMixObject(), 1);
        return;
    }
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_nativeaudio_NativeAudio_stopPlay(JNIEnv *env, jclass clazz) {

//    VOICEENGINEAPI::DestroyDecodeChannel(0);
//    VOICEENGINEAPI::DestroyDecodeChannel(1);

LOGD("zsyzsy  stop player");
    MyVoiceEngineInterface::GetVoiceEngine()->StopPlayout(0);
    MyVoiceEngineInterface::GetVoiceEngine()->DestroyDecodeChannel(0);

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_nativeaudio_NativeAudio_playAudio(JNIEnv *env,
                                                   jclass clazz) {
//    int channelid = VOICEENGINEAPI::CreateDecodeChannel(VOICEENGINEAPI::CODEC_G7221_32K);
//    VOICEENGINEAPI::StartPlayout(channelid);

    int channelid = MyVoiceEngineInterface::GetVoiceEngine()->CreateDecodeChannel();
    MyVoiceEngineInterface::GetVoiceEngine()->StartPlayout(channelid);
    if (pcmFileRead) {
        LOGD("zsy pcmFileRead!=null");
    }
    ProduceData();

//    VOICEENGINEAPI::CreateDecodeChannel(VOICEENGINEAPI::CODEC_G7221_32K);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_nativeaudio_NativeAudio_stopRecord(JNIEnv *env,
                                                    jclass clazz) {


    MyVoiceEngineInterface::GetVoiceEngine()->DeRegisterAudioCallback();

    MyVoiceEngineInterface::GetVoiceEngine()->StopRecording();

    if (g_pRecordData) {
        delete g_pRecordData;
        g_pRecordData = NULL;
    }


    if (g_pRecord) {
        g_pRecord->StopRecord();
        g_pRecord->ReleaseRecord();
    }
}

// create audio recorder: recorder is not in fast path
//    like to avoid excessive re-sampling while playing back from Hello & Android clip
extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_nativeaudio_NativeAudio_createAudioRecorder(JNIEnv *env, jclass clazz) {
    if (g_bUseOpenCore) {

        MyVoiceEngineInterface::GetVoiceEngine()->SetSendCodec(CODEC_G7221_32K);


        return true;
    }


    return JNI_TRUE;
}


// set the recording state for the audio recorder
extern "C" JNIEXPORT void JNICALL
Java_com_example_nativeaudio_NativeAudio_startRecording(JNIEnv *env, jclass clazz) {
    if (g_bUseOpenCore) {
        if (NULL == g_pRecordData) {
            g_pRecordData = new RecordData();
        }
        MyVoiceEngineInterface::GetVoiceEngine()->RegisterAudioCallback(g_pRecordData);
        MyVoiceEngineInterface::GetVoiceEngine()->StartRecording();



        if (g_pRecord) {
            g_pRecord->StartRecord();
        }
        return;
    }

}


// shut down the native audio system
extern "C" JNIEXPORT void JNICALL
Java_com_example_nativeaudio_NativeAudio_shutdown(JNIEnv *env, jclass clazz) {

    MyVoiceEngineInterface::DestroyVoiceEngine(MyVoiceEngineInterface::GetVoiceEngine());
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerMuteSolo = NULL;
        bqPlayerVolume = NULL;
    }

    // destroy file descriptor audio player object, and invalidate all associated interfaces
    if (fdPlayerObject != NULL) {
        (*fdPlayerObject)->Destroy(fdPlayerObject);
        fdPlayerObject = NULL;
        fdPlayerPlay = NULL;
        fdPlayerSeek = NULL;
        fdPlayerMuteSolo = NULL;
        fdPlayerVolume = NULL;
    }

    // destroy URI audio player object, and invalidate all associated interfaces
    if (uriPlayerObject != NULL) {
        (*uriPlayerObject)->Destroy(uriPlayerObject);
        uriPlayerObject = NULL;
        uriPlayerPlay = NULL;
        uriPlayerSeek = NULL;
        uriPlayerMuteSolo = NULL;
        uriPlayerVolume = NULL;
    }

    // destroy audio recorder object, and invalidate all associated interfaces
    if (recorderObject != NULL) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = NULL;
        recorderRecord = NULL;
        recorderBufferQueue = NULL;
    }
    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    pthread_mutex_destroy(&audioEngineLock);
}
