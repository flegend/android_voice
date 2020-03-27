//
// Created by root on 19-5-15.
//

#ifndef NATIVE_AUDIO_OPENSLESCORE_H
#define NATIVE_AUDIO_OPENSLESCORE_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "../include/my_voice_engine_types.h"
#include "XUtil/XDataPool.h"

namespace VOICEENGINEAPI {

    class CAudioEngine {
    public:
        CAudioEngine();

        ~CAudioEngine();

        SLresult CreateEngine();

        SLEngineItf GetEngine();

        SLresult ReleaseEngine();

        SLObjectItf GetOutputMixObject();

        SLresult ReleaseOutputMixObject();

    private:
        SLObjectItf m_pEngineObject;
        SLEngineItf m_pEngineEngine;

        SLObjectItf m_pOutputMixObject;
    };

    class CAudioRecord {
    public:
        CAudioRecord(SLEngineItf pEngine);

        ~CAudioRecord();

        SLresult
        CreateAudioRecord(SLuint32 numChannels, SLuint32 samplesPerSec, SLuint32 bitsPerSample);

        SLresult StartRecord();

        SLresult StopRecord();

        SLresult ReleaseRecord();

        SLresult GetRecordState();

        void SetRecordDataCallback(Transport *pRecordDataCallback);

    private:
        static void BqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    private:
        SLEngineItf m_pEngineEngine;

        SLObjectItf m_pRecorderObject;
        SLRecordItf m_pRecorderRecord;
        SLAndroidSimpleBufferQueueItf m_pRecorderBufferQueue;
        uint32_t m_nRecordSize;
        int8_t *m_pPCMDataBuffer;
        Transport *m_pRecordDataCallback;
    };

    class CAudioPlayer {
    public:
        CAudioPlayer(SLEngineItf pEngine, SLObjectItf pOutputMixObject);

        ~CAudioPlayer();

        SLresult
        CreateBufferQueueAudioPlayer(SLuint32 numChannels, SLuint32 samplesPerSec,
                                     SLuint32 bitsPerSample);

        SLresult PlayAudio(const int8_t *pData, int32_t nLen);

        SLresult StartPlayer();

        SLresult StopPlayer();

        SLresult ReleasePlayer();

        SLresult GetPlayerState();

    private:
        void FillBufferQueue();
        void EnqueuePlayoutData(bool silence);
    private:
        static void BqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    private:
        SLObjectItf m_pOutputMixObject;
        SLEngineItf m_pEngineEngine;
        SLObjectItf m_pPlayerObject;
        SLPlayItf m_pPlayerPlay;
        SLAndroidSimpleBufferQueueItf m_pPlayerBufferQueue;

        XDataPool m_AudioDataPool;

        uint8_t* m_pAudioBuf;
        int32_t m_nAudioBufLen;
    };
}
#endif //NATIVE_AUDIO_OPENSLESCORE_H
