//
// Created by zsy on 2019/5/20.
//
#include "PlayerChannel.h"
#include "native_debug.h"
#include "XUtil/XUtil.h"
#include "VoiceMgr.h"
#include "CodecMgr.h"
#include "../include/rtp_header.h"
#include "XUtil/OperationMgr.h"
#include "XUtil/XAutoLock.h"

namespace VOICEENGINEAPI {
    CPlayerChannel::CPlayerChannel(CAudioEngine *engine) :
            m_pAudioEngine(NULL),
            m_pAudioPlayer(NULL),
            m_pCodec(NULL),
            m_nCodeId(CODEC_G7221_32K),
            m_nChannelType(CHANNEL_OTHER),
            m_bPlaying(false),
            m_nChannelId(0) {
        ASSERT(NULL != engine, "CPlayerChannel engine can not NULL");
        m_pAudioEngine = engine;
        m_pAudioPlayer = new CAudioPlayer(m_pAudioEngine->GetEngine(),
                                          m_pAudioEngine->GetOutputMixObject());

        m_AudioDataPool.FlushPool();
    }

    CPlayerChannel::~CPlayerChannel() {
        StopPlayout();
        if (NULL != m_pAudioPlayer) {
            delete m_pAudioPlayer;
            m_pAudioPlayer = NULL;
            m_pAudioEngine = NULL;
        }

        CloseCodec();
    }

    void CPlayerChannel::ChannelInit(ChannelType type) {
        m_nChannelType = type;
    }

    void CPlayerChannel::ThreadProcMain(void) {
        while (m_bPlaying) {
            XDataPool::XDataBuffer *pDataBuffer = m_AudioDataPool.GetDataBuffer();
            if (pDataBuffer) {
                DoDecode((int8_t *) pDataBuffer->pBuffer, pDataBuffer->nLen);
                pDataBuffer->Release();
                pDataBuffer = NULL;
            } else {
                XSleep(2);
            }
        }
    }

    void CPlayerChannel::SetChannlID(int32_t nChannelId) {
        m_nChannelId = nChannelId;
    }

    int32_t CPlayerChannel::GetChannelId() {
        return m_nChannelId;
    }

    int32_t CPlayerChannel::SetCodec(VOICE_ENGINE_CODEC_ID codec_id) {
        m_nCodeId = codec_id;
        CodecInst inst;
        CVoiceMgr::Ins().GetCodecInfo(m_nCodeId, inst);
        LOGD("CPlayerChannel SetCodec CodecInst[pltype=%d, plname=%s]", inst.pltype,
             inst.plname);
        CloseCodec();
        m_pCodec = CodecMgr::Ins().CreateCodec((CodecType) inst.pltype);
        InitDecode(inst.rate, inst.plfreq);
        return 0;
    }

    int32_t CPlayerChannel::Decode(const int8_t *data, int32_t nlen) {
        if (m_bPlaying) {
            m_AudioDataPool.PushBack((void *) data, nlen);
        }
        return 0;
    }

    int32_t CPlayerChannel::StartPlayout() {
        if (m_bPlaying) {
            LOGW("CPlayerChannel StartPlayout already");
            return 0;
        }

        if (NULL != m_pAudioPlayer) {

            CreatePlayer();

            m_bPlaying = true;
            if (!XThreadBase::StartThread()) {
                m_bPlaying = false;
                LOGE("CPlayerChannel StartPlayout StartThread error");
                return -1;
            }
            std::string thread_name = "PlayerChannel" + XInt2Str(m_nChannelId);
            SetName(thread_name.c_str());
        }

        return 0;
    }

    int32_t CPlayerChannel::StopPlayout() {
        if (m_bPlaying) {
            m_bPlaying = false;
            XThreadBase::WaitForStop();
            m_AudioDataPool.FlushPool();

            ReleasePlayer();
        }

        return 0;
    }

    int32_t CPlayerChannel::CreatePlayer()
    {
        XAutoLock lockPlayer(m_csPlayer);
        if (NULL != m_pCodec && NULL != m_pAudioPlayer) {
            LOGD("CPlayerChannel::CreatePlayer()");
            CodecInst inst;
            CVoiceMgr::Ins().GetCodecInfo(m_nCodeId, inst);

            SLuint32 numChannels = inst.channels;
            SLuint32 samplesPerSec = inst.plfreq * 1000;
            SLuint32 bitsPerSample = 16;
            m_pAudioPlayer->CreateBufferQueueAudioPlayer(numChannels, samplesPerSec, bitsPerSample);
            m_pAudioPlayer->StartPlayer();
        }

        return 0;
    }

    int32_t CPlayerChannel::ReleasePlayer()
    {
        XAutoLock lockPlayer(m_csPlayer);
        if (NULL != m_pAudioPlayer) {
            LOGD("CPlayerChannel::ReleasePlayer()");
            m_pAudioPlayer->StopPlayer();
            m_pAudioPlayer->ReleasePlayer();
        }
        return 0;
    }

    bool CPlayerChannel::CloseCodec() {
        if (NULL != m_pCodec) {
            m_pCodec->DecodeUnint();
            delete m_pCodec;
            m_pCodec = NULL;
        }
        return true;
    }

    bool CPlayerChannel::InitDecode(int32_t bit_rate, int32_t sample_rate) {
        if (NULL != m_pCodec) {
            m_pCodec->DecodeUnint();
            m_pCodec->DecodeInit(bit_rate, sample_rate);
        }
        return true;
    }

    void CPlayerChannel::DoDecode(const int8_t *pData, int32_t nLen) {

        if (nLen > 0 && NULL != pData) {
            PRTP_HEADER pRTPHeader = (PRTP_HEADER) (const uint8_t *) pData;
            int32_t pltype = pRTPHeader->type;

            if (NULL != m_pCodec) {
                CodecInst inst;
                CVoiceMgr::Ins().GetCodecInfo(m_nCodeId, inst);
                if (inst.pltype != pltype) {
                    ReleasePlayer();
                    SetCodec(CVoiceMgr::Ins().GetCodecId(pltype));
                    CreatePlayer();
                }
            } else {
                SetCodec(CVoiceMgr::Ins().GetCodecId(pltype));
                CreatePlayer();
            }

            int8_t output[1500] = {0};
            int32_t output_len = 0;
            m_pCodec->DecodeAudio(pData + RTP_HEADER_LEN, nLen - RTP_HEADER_LEN, output,
                                  output_len);
            if (output_len > 0) {
                OnDecodeData(output, output_len);
            }
        }
    }

    void CPlayerChannel::OnDecodeData(const int8_t *pData, int32_t nLen) {
//        LOGD("CPlayerChannel OnDecodeData nLen = %d", nLen);
        if (NULL != m_pAudioPlayer) {
            m_pAudioPlayer->PlayAudio(pData, nLen);
        }
    }
}
