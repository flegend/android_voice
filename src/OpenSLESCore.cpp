//
// Created by root on 19-5-15.
//
#include <malloc.h>
#include "OpenSLESCore.h"
#include "native_debug.h"
#include "XUtil/XAutoLock.h"
#include "XUtil/XUtil.h"
#include <stdio.h>

#define kNumOfOpenSLESBuffers  2

namespace VOICEENGINEAPI {

    CAudioEngine::CAudioEngine() {
        m_pEngineObject = NULL;
        m_pEngineEngine = NULL;
        m_pOutputMixObject = NULL;
        CreateEngine();
    }

    CAudioEngine::~CAudioEngine() {
        ReleaseEngine();
    }

    SLresult CAudioEngine::CreateEngine() {
        LOGD("CAudioEngine CreateEngine");

        if (NULL != m_pEngineEngine) {
            LOGD("CAudioEngine Engine created already");
            return SL_RESULT_SUCCESS;
        }

        SLresult result;
        // create engine
        result = slCreateEngine(&m_pEngineObject, 0, NULL, 0, NULL, NULL);
        ASSERT(SL_RESULT_SUCCESS == result, "result = %d", result);
        (void) result;

        // realize the engine
        result = (*m_pEngineObject)->Realize(m_pEngineObject, SL_BOOLEAN_FALSE);
        ASSERT(SL_RESULT_SUCCESS == result, "result = %d", result);
        (void) result;

        // get the engine interface, which is needed in order to create other objects
        result = (*m_pEngineObject)->GetInterface(m_pEngineObject, SL_IID_ENGINE, &m_pEngineEngine);
        ASSERT(SL_RESULT_SUCCESS == result, "result = %d", result);
        (void) result;

        return SL_RESULT_SUCCESS;
    }

    SLEngineItf CAudioEngine::GetEngine() {
        return m_pEngineEngine;
    }

    SLObjectItf CAudioEngine::GetOutputMixObject() {
        if (NULL != m_pEngineEngine && NULL == m_pOutputMixObject) {
            LOGI("CAudioEngine CreateOutMix");
            // create output mix
            SLresult result;
            const SLboolean req[1] = {SL_BOOLEAN_FALSE};
            result = (*m_pEngineEngine)->CreateOutputMix(m_pEngineEngine, &m_pOutputMixObject, 0, 0,
                                                         req);
            ASSERT(SL_RESULT_SUCCESS == result, "CreateOutMix CreateOutputMix error");
            (void) result;

            // realize the output mix
            result = (*m_pOutputMixObject)->Realize(m_pOutputMixObject, SL_BOOLEAN_FALSE);
            ASSERT(SL_RESULT_SUCCESS == result, "CreateOutMix Realize error");
            (void) result;
        }

        return m_pOutputMixObject;
    }

    SLresult CAudioEngine::ReleaseOutputMixObject()
    {
        LOGI("CAudioEngine ReleaseOutputMixObject");
        if (NULL != m_pOutputMixObject) {
            (*m_pOutputMixObject)->Destroy(m_pOutputMixObject);
            m_pOutputMixObject = NULL;
        }
        return SL_RESULT_SUCCESS;
    }


    SLresult CAudioEngine::ReleaseEngine() {
        ReleaseOutputMixObject();

        LOGI("CAudioEngine ReleaseEngine");
        if (NULL != m_pEngineObject) {
            (*m_pEngineObject)->Destroy(m_pEngineObject);
            m_pEngineObject = NULL;
            m_pEngineEngine = NULL;
        }

        return SL_RESULT_SUCCESS;
    }

    CAudioRecord::CAudioRecord(SLEngineItf pEngine) :
            m_pRecorderObject(NULL),
            m_pRecorderRecord(NULL),
            m_pRecorderBufferQueue(NULL),
            m_nRecordSize(0),
            m_pPCMDataBuffer(NULL),
            m_pRecordDataCallback(NULL) {
        m_pEngineEngine = pEngine;
    }

    CAudioRecord::~CAudioRecord() {
        ReleaseRecord();
    }

    SLresult
    CAudioRecord::CreateAudioRecord(SLuint32 numChannels, SLuint32 samplesPerSec,
                                    SLuint32 bitsPerSample) {
        ASSERT(NULL != m_pEngineEngine, "CAudioRecord CreateAudioRecord not create SLEngineItf");
        LOGI("CAudioRecord CreateAudioRecord(numChannels = %d, samplesPerSec = %d, bitsPerSample = %d)",
             numChannels, samplesPerSec, bitsPerSample);

        SLresult result;

        // configure audio source
        SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT,
                                          SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
        SLDataSource audioSrc = {&loc_dev, NULL};

        // configure audio sink
        SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                         kNumOfOpenSLESBuffers};
        SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
                                       numChannels,
                                       samplesPerSec,
                                       bitsPerSample,
                                       bitsPerSample,
                                       numChannels == 2 ? (SL_SPEAKER_FRONT_LEFT |
                                                           SL_SPEAKER_FRONT_RIGHT)
                                                        : SL_SPEAKER_FRONT_CENTER,
                                       SL_BYTEORDER_LITTLEENDIAN};
        SLDataSink audioSnk = {&loc_bq, &format_pcm};

        // create audio recorder
        // (requires the RECORD_AUDIO permission)
        const SLInterfaceID id[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_ANDROIDCONFIGURATION};
        const SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
        result = (*m_pEngineEngine)->CreateAudioRecorder(m_pEngineEngine, &m_pRecorderObject,
                                                         &audioSrc,
                                                         &audioSnk, sizeof(id) / sizeof(id[0]), id, req);
        ASSERT(SL_RESULT_SUCCESS == result, "CreateAudioRecorder, result = %d", result);
        (void) result;

        // Configure the voice recognition preset which has no
        // signal processing for lower latency.
        SLAndroidConfigurationItf recorder_config;
        result = (*m_pRecorderObject)
                   ->GetInterface(m_pRecorderObject, SL_IID_ANDROIDCONFIGURATION,
                                  &recorder_config);
        if (SL_RESULT_SUCCESS == result)
        {
            SLuint32 presetValue = SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION;
            (*recorder_config)->SetConfiguration(recorder_config, SL_ANDROID_KEY_RECORDING_PRESET,
                                   &presetValue, sizeof(SLuint32));
        }

        // realize the audio recorder
        result = (*m_pRecorderObject)->Realize(m_pRecorderObject, SL_BOOLEAN_FALSE);
        ASSERT(SL_RESULT_SUCCESS == result, "Realize, result = %d", result);
        (void) result;

        // get the record interface
        result = (*m_pRecorderObject)->GetInterface(m_pRecorderObject, SL_IID_RECORD,
                                                    &m_pRecorderRecord);
        ASSERT(SL_RESULT_SUCCESS == result, "GetInterface, result = %d", result);
        (void) result;

        // get the buffer queue interface
        result = (*m_pRecorderObject)->GetInterface(m_pRecorderObject,
                                                    SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                    &m_pRecorderBufferQueue);
        ASSERT(SL_RESULT_SUCCESS == result, "GetInterface, result = %d",
               result);
        (void) result;

        // register callback on the buffer queue
        result = (*m_pRecorderBufferQueue)->RegisterCallback(m_pRecorderBufferQueue,
                                                             BqRecorderCallback,
                                                             this);
        ASSERT(SL_RESULT_SUCCESS == result,
               "RegisterCallback, result = %d",
               result);
        (void) result;

        m_nRecordSize = numChannels * (samplesPerSec / 1000 / 50);
        if (bitsPerSample == SL_PCMSAMPLEFORMAT_FIXED_32) {
            m_nRecordSize = m_nRecordSize * 4;
        } else if (bitsPerSample == SL_PCMSAMPLEFORMAT_FIXED_16) {
            m_nRecordSize = m_nRecordSize * 2;
        }
        m_pPCMDataBuffer = static_cast<int8_t *>(malloc(sizeof(int8_t) * m_nRecordSize));

        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioRecord::StartRecord() {
        ASSERT(NULL != m_pRecorderRecord, "StartRecord not create SLRecordItf");
        ASSERT(NULL != m_pRecorderBufferQueue,
               "StartRecord not create SLAndroidSimpleBufferQueueItf");

        LOGD("CAudioRecord StartRecord");
        if (GetRecordState() == SL_RECORDSTATE_RECORDING) {
            LOGW("CAudioRecord StartRecord already recording");
            return SL_RESULT_SUCCESS;
        }

        SLresult result;
        result = (*m_pRecorderBufferQueue)->Clear(m_pRecorderBufferQueue);
        ASSERT(SL_RESULT_SUCCESS == result, "");
        (void) result;

        // enqueue an empty buffer to be filled by the recorder
        result = (*m_pRecorderBufferQueue)->Enqueue(m_pRecorderBufferQueue, m_pPCMDataBuffer,
                                                    m_nRecordSize);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        ASSERT(SL_RESULT_SUCCESS == result, "Enqueue result = %d", result);
        (void) result;

        // start recording
        result = (*m_pRecorderRecord)->SetRecordState(m_pRecorderRecord, SL_RECORDSTATE_RECORDING);
        ASSERT(SL_RESULT_SUCCESS == result, "SetRecordState result = %d", result);
        (void) result;
        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioRecord::StopRecord() {
        LOGD("CAudioRecord StopRecord");
        if (!m_pRecorderRecord) {
            LOGD("CAudioRecord StopRecord fail. not create SLRecordItf");
            return SL_RESULT_UNKNOWN_ERROR;
        }

        if (GetRecordState() == SL_RECORDSTATE_RECORDING) {
            (*m_pRecorderRecord)->SetRecordState(m_pRecorderRecord, SL_RECORDSTATE_STOPPED);
        } else {
            LOGD("CAudioRecord StopRecord fail. state = %d", GetRecordState());
        }

        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioRecord::ReleaseRecord() {
        LOGD("CAudioRecord ReleaseRecord");
        if (GetRecordState() == SL_RECORDSTATE_RECORDING) {
            StopRecord();
        }

        if (NULL != m_pPCMDataBuffer) {
            free(m_pPCMDataBuffer);
            m_pPCMDataBuffer = NULL;
        }

        if (NULL != m_pRecorderObject) {
            (*m_pRecorderObject)->Destroy(m_pRecorderObject);
            m_pRecorderObject = NULL;
            m_pRecorderRecord = NULL;
            m_pRecorderBufferQueue = NULL;
        }

        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioRecord::GetRecordState() {
        if (NULL != m_pRecorderRecord) {
            SLresult result;
            SLuint32 recordState;
            result = (*m_pRecorderRecord)->GetRecordState(m_pRecorderRecord, &recordState);
            if (result == SL_RESULT_SUCCESS) {
                return recordState;
            }
        }
        return SL_RECORDSTATE_STOPPED;
    }

    void CAudioRecord::SetRecordDataCallback(Transport *pRecordDataCallback) {
        m_pRecordDataCallback = pRecordDataCallback;
    }

    void CAudioRecord::BqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
        CAudioRecord *record = static_cast<CAudioRecord *>(context);
        if (NULL != record && record->GetRecordState() == SL_RECORDSTATE_RECORDING) {
            if (NULL != record->m_pRecordDataCallback) {
                record->m_pRecordDataCallback->SendPacket(0, record->m_pPCMDataBuffer,
                                                          record->m_nRecordSize);
            }

            (*record->m_pRecorderBufferQueue)->Enqueue(record->m_pRecorderBufferQueue,
                                                       record->m_pPCMDataBuffer,
                                                       record->m_nRecordSize);
        }
    }

    CAudioPlayer::CAudioPlayer(SLEngineItf pEngine, SLObjectItf pOutputMixObject) :
            m_pPlayerObject(NULL),
            m_pPlayerPlay(NULL),
            m_pPlayerBufferQueue(NULL) {
        m_pEngineEngine = pEngine;
        m_pOutputMixObject = pOutputMixObject;
        m_pAudioBuf = NULL;
        m_nAudioBufLen = 0;
    }

    CAudioPlayer::~CAudioPlayer() {
        ReleasePlayer();
    }

    SLresult
    CAudioPlayer::CreateBufferQueueAudioPlayer(SLuint32 numChannels, SLuint32 samplesPerSec,
                                               SLuint32 bitsPerSample) {
        ASSERT(NULL != m_pEngineEngine,
               "CAudioPlayer CreateBufferQueueAudioPlayer not create SLEngineItf");
        ASSERT(NULL != m_pOutputMixObject,
               "CAudioPlayer CreateBufferQueueAudioPlayer not create OutMix");

        LOGI("CAudioPlayer CreateBufferQueueAudioPlayer(numChannels = %d, samplesPerSec = %d, bitsPerSample = %d)",
             numChannels, samplesPerSec, bitsPerSample);

        SLresult result;

        // configure audio source
        SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                           kNumOfOpenSLESBuffers};
        SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, numChannels, samplesPerSec,
                                       bitsPerSample, bitsPerSample,
                                       numChannels == 2 ? (SL_SPEAKER_FRONT_LEFT |
                                                           SL_SPEAKER_FRONT_RIGHT)
                                                        : SL_SPEAKER_FRONT_CENTER,
                                       SL_BYTEORDER_LITTLEENDIAN};
        SLDataSource audioSrc = {&loc_bufq, &format_pcm};

        // configure audio sink
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, m_pOutputMixObject};
        SLDataSink audioSnk = {&loc_outmix, NULL};

        // create audio player
        const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE, SL_IID_ANDROIDCONFIGURATION};
        const SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

        result = (*m_pEngineEngine)->CreateAudioPlayer(m_pEngineEngine, &m_pPlayerObject, &audioSrc,
                                                       &audioSnk,
                                                       sizeof(ids) / sizeof(ids[0]), ids,
                                                       req);
        ASSERT(SL_RESULT_SUCCESS == result, "CreateAudioPlayer error.result = %d", result);
        (void) result;

        // Use the Android configuration interface to set platform-specific
        // parameters. Should be done before player is realized.
        SLAndroidConfigurationItf player_config;
        result = (*m_pPlayerObject)->GetInterface(m_pPlayerObject, SL_IID_ANDROIDCONFIGURATION, &player_config);
        if (SL_RESULT_SUCCESS == result)
        {
            // Set audio player configuration to SL_ANDROID_STREAM_VOICE which
            // corresponds to android.media.AudioManager.STREAM_VOICE_CALL.
            SLint32 stream_type = SL_ANDROID_STREAM_VOICE;
            (*player_config)->SetConfiguration(player_config, SL_ANDROID_KEY_STREAM_TYPE, &stream_type, sizeof(SLint32));
        }

        // realize the player
        result = (*m_pPlayerObject)->Realize(m_pPlayerObject, SL_BOOLEAN_FALSE);
        ASSERT(SL_RESULT_SUCCESS == result, "Realize error.result = %d", result);
        (void) result;

        // get the play interface
        result = (*m_pPlayerObject)->GetInterface(m_pPlayerObject, SL_IID_PLAY, &m_pPlayerPlay);
        ASSERT(SL_RESULT_SUCCESS == result, "GetInterface error.result = %d",
               result);
        (void) result;

        // get the buffer queue interface
        result = (*m_pPlayerObject)->GetInterface(m_pPlayerObject, SL_IID_BUFFERQUEUE,
                                                  &m_pPlayerBufferQueue);
        ASSERT(SL_RESULT_SUCCESS == result,
               "CreateAudioPlayer GetInterface PlayerBufferQueue error.result = %d", result);
        (void) result;

        // register callback on the buffer queue
        result = (*m_pPlayerBufferQueue)->RegisterCallback(m_pPlayerBufferQueue, BqPlayerCallback,
                                                           this);
        ASSERT(SL_RESULT_SUCCESS == result, "RegisterCallback error. result = %d", result);
        (void) result;

        m_nAudioBufLen = numChannels * (samplesPerSec / 1000 / 50);
        if (bitsPerSample == SL_PCMSAMPLEFORMAT_FIXED_32) {
            m_nAudioBufLen = m_nAudioBufLen * 4;
        } else if (bitsPerSample == SL_PCMSAMPLEFORMAT_FIXED_16) {
            m_nAudioBufLen = m_nAudioBufLen * 2;
        }

        m_pAudioBuf = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * m_nAudioBufLen));
        memset(m_pAudioBuf, 0, m_nAudioBufLen);

        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioPlayer::PlayAudio(const int8_t *pData, int32_t nLen) {
        if (GetPlayerState() == SL_PLAYSTATE_PLAYING) {
            m_AudioDataPool.PushBack((void *) pData, nLen);
            if (m_AudioDataPool.GetDataCount() > 50) {
                LOGD("PlayAudio m_AudioDataPool count > 50");
                XSleep(20);
            }
        }
        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioPlayer::StartPlayer() {
        LOGD("CAudioPlayer StartPlayer");
        if (NULL != m_pPlayerPlay) {
            for (int32_t i = 0; i < kNumOfOpenSLESBuffers; ++i) {
                EnqueuePlayoutData(true);
            }

            SLresult result;
            // set the player's state to playing
            result = (*m_pPlayerPlay)->SetPlayState(m_pPlayerPlay, SL_PLAYSTATE_PLAYING);
            ASSERT(SL_RESULT_SUCCESS == result, "SetPlayState playing error. result = %d", result);
            (void) result;
        }

        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioPlayer::StopPlayer() {
        LOGD("CAudioPlayer StopPlayer");
        if (GetPlayerState() == SL_PLAYSTATE_PLAYING) {
            SLresult result;
            result = (*m_pPlayerPlay)->SetPlayState(m_pPlayerPlay, SL_PLAYSTATE_STOPPED);
            ASSERT(SL_RESULT_SUCCESS == result, "SetPlayState playing error. result = %d", result);
            (*m_pPlayerBufferQueue)->Clear(m_pPlayerBufferQueue);
        }

        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioPlayer::ReleasePlayer() {
        LOGD("CAudioPlayer ReleasePlayer");
        if (m_pPlayerObject != NULL) {

            (*m_pPlayerBufferQueue)
                  ->RegisterCallback(m_pPlayerBufferQueue, NULL, NULL);

            (*m_pPlayerObject)->Destroy(m_pPlayerObject);
            m_pPlayerObject = NULL;
            m_pPlayerPlay = NULL;
            m_pPlayerBufferQueue = NULL;
        }

        if (NULL != m_pAudioBuf) {
            free(m_pAudioBuf);
            m_pAudioBuf = NULL;
        }

        return SL_RESULT_SUCCESS;
    }

    SLresult CAudioPlayer::GetPlayerState() {
        if (NULL != m_pPlayerPlay) {
            SLresult result;
            SLuint32 playState;
            result = (*m_pPlayerPlay)->GetPlayState(m_pPlayerPlay, &playState);
            if (SL_RESULT_SUCCESS == result) {
                return playState;
            }
        }
        return SL_PLAYSTATE_STOPPED;
    }

    void CAudioPlayer::FillBufferQueue()
    {
        if (GetPlayerState() == SL_PLAYSTATE_PLAYING)
        {
            EnqueuePlayoutData(false);
        }
    }

    void CAudioPlayer::EnqueuePlayoutData(bool silence)
    {
        if (!silence)
        {
            XDataPool::XDataBuffer *pDataBuffer = m_AudioDataPool.GetDataBuffer();
            if (pDataBuffer) {
                if (pDataBuffer->nLen <= m_nAudioBufLen)
                {
                    memcpy(m_pAudioBuf, pDataBuffer->pBuffer, pDataBuffer->nLen);
                    SLresult err = (*m_pPlayerBufferQueue)->Enqueue(m_pPlayerBufferQueue, m_pAudioBuf, pDataBuffer->nLen);
                    if (SL_RESULT_SUCCESS != err) {
                        LOGE("Enqueue failed: %d", err);
                    }
                }

                pDataBuffer->Release();
                pDataBuffer = NULL;
            }
            else
            {
                silence = true;
            }
        }

        if (silence)
        {
            memset(m_pAudioBuf, 0, m_nAudioBufLen);
            (*m_pPlayerBufferQueue)->Enqueue(m_pPlayerBufferQueue, m_pAudioBuf, m_nAudioBufLen);
        }
    }

    // static
    void CAudioPlayer::BqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
        CAudioPlayer *player = reinterpret_cast<CAudioPlayer *>(context);
        if (NULL != player) {
            player->FillBufferQueue();
        }
    }
}
