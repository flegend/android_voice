//
// Created by zsy on 2019/5/20.
//

#include "RecordChannel.h"
#include "native_debug.h"
#include "XUtil/XUtil.h"
#include "VoiceMgr.h"
#include "CodecMgr.h"
#include "XUtil/XAutoLock.h"
#include "../include/rtp_header.h"
#include "../include/AUDEC_Header.h"

namespace VOICEENGINEAPI {
    CRecordChannel::CRecordChannel(CAudioEngine *engine) :
            m_pAudioEngine(NULL),
            m_pAudioRecord(NULL),
            m_pCallBack(NULL),
            m_pCodec(NULL),
            m_nCodeId(CODEC_G7221_32K),
            m_nChannelType(CHANNEL_OTHER),
            m_bRecording(false),
            m_nRtpTimestamp(0),
            m_nRtpSeqNo(0) {
        ASSERT(NULL != engine, "CRecordChannel engine can not NULL");
        m_pAudioEngine = engine;
        m_pAudioRecord = new CAudioRecord(m_pAudioEngine->GetEngine());
        m_pAudioRecord->SetRecordDataCallback(this);

        m_AudioDataPool.FlushPool();
    }

    CRecordChannel::~CRecordChannel() {
        if (NULL != m_pAudioRecord) {
            StopRecording();
            m_pAudioRecord = NULL;
            m_pAudioEngine = NULL;
            m_pCallBack = NULL;
        }

        CloseCodec();
    }

    void CRecordChannel::ChannelInit(ChannelType type) {
        m_nChannelType = type;
    }

    void CRecordChannel::ThreadProcMain(void) {
        while (m_bRecording) {
            XDataPool::XDataBuffer *pDataBuffer = m_AudioDataPool.GetDataBuffer();
            if (pDataBuffer) {
                DoEncode((int8_t *) pDataBuffer->pBuffer, pDataBuffer->nLen);
                pDataBuffer->Release();
                pDataBuffer = NULL;
            } else {
                XSleep(2);
            }
        }
    }

    int CRecordChannel::SendPacket(int channel, const void *data, size_t len) {
        // callback pcm data
        m_AudioDataPool.PushBack((void *) data, len);
        return 0;
    }

    int32_t CRecordChannel::SetSendCodec(VOICE_ENGINE_CODEC_ID codec_id) {
        m_nCodeId = codec_id;
        CodecInst inst;
        CVoiceMgr::Ins().GetCodecInfo(m_nCodeId, inst);
        LOGD("CRecordChannel SetSendCodec CodecInst[pltype=%d, plname=%s]", inst.pltype,
             inst.plname);
        CloseCodec();
        m_pCodec = CodecMgr::Ins().CreateCodec((CodecType) inst.pltype);
        InitEncode(inst.rate, inst.plfreq);
        return 0;
    }

    int32_t CRecordChannel::RegisterAudioCallback(Transport *cb) {
        XAutoLock l(m_csCallback);
        m_pCallBack = cb;
        return 0;
    }

    int32_t CRecordChannel::DeRegisterAudioCallback() {
        XAutoLock l(m_csCallback);
        m_pCallBack = NULL;
        if (NULL != m_pAudioRecord) {
//            m_pAudioRecord->SetRecordDataCallback(NULL);
        }
        return 0;
    }

    int32_t CRecordChannel::StartRecording() {
        ASSERT(NULL != m_pCodec, "CRecordChannel Codec isNULL");

        if (m_bRecording) {
            LOGW("CRecordChannel StartRecording already");
            return 0;
        }

        if (NULL != m_pAudioRecord) {
            CodecInst inst;
            CVoiceMgr::Ins().GetCodecInfo(m_nCodeId, inst);

            SLuint32 numChannels = inst.channels;
            SLuint32 samplesPerSec = inst.plfreq * 1000;
            SLuint32 bitsPerSample = 16;
            m_pAudioRecord->CreateAudioRecord(numChannels, samplesPerSec, bitsPerSample);
            SLresult result = m_pAudioRecord->StartRecord();

            if (result == SL_RESULT_SUCCESS) {
                m_bRecording = true;
                if (!XThreadBase::StartThread()) {
                    m_bRecording = false;
                    LOGE("CRecordChannel StartRecording StartThread error");
                    return -1;
                }
                SetName("RecordChannel");
            } else {
                LOGE("CRecordChannel StartRecording Error, result = %d", result);
            }
        }
        return 0;
    }

    int32_t CRecordChannel::StopRecording() {
        if (NULL != m_pAudioRecord) {
            m_pAudioRecord->StopRecord();
            m_pAudioRecord->ReleaseRecord();
        }

        m_bRecording = false;
        XThreadBase::WaitForStop();
        m_AudioDataPool.FlushPool();
        return 0;
    }

    bool CRecordChannel::CloseCodec() {
        if (NULL != m_pCodec) {
            m_pCodec->EncodeUnint();
            delete m_pCodec;
            m_pCodec = NULL;
        }
        return true;
    }

    bool CRecordChannel::InitEncode(int32_t bit_rate, int32_t sample_rate) {
        if (NULL != m_pCodec) {
            m_pCodec->EncodeUnint();
            m_pCodec->EncodeInit(bit_rate, sample_rate);
        }
        return true;
    }

    void CRecordChannel::DoEncode(const int8_t *pData, int32_t nLen) {
        if (NULL != m_pCodec) {
            int8_t output[512] = {0};
            int32_t output_len = 0;
            m_pCodec->EncodeAudio(pData, nLen, output, output_len);
            if (output_len > 0) {
                OnEncodeData(output, output_len);
            }
        }
    }

    void CRecordChannel::OnEncodeData(const int8_t *pData, int32_t nLen) {
//        LOGD("CRecordChannel::OnEncodeData nLen = %d", nLen);
        XAutoLock l(m_csCallback);
        if (NULL != m_pCallBack) {
            uint8_t pRtpPacket[1500] = {0};
            int32_t rtpPacketLen = AddRTPHeader(pRtpPacket, (const uint8_t *) pData, nLen);
            m_pCallBack->SendPacket(0, pRtpPacket, rtpPacketLen);
        }
    }

    int32_t CRecordChannel::AddRTPHeader(uint8_t *pRTPPacket, const uint8_t *pData, int32_t nLen) {
        CodecInst inst;
        CVoiceMgr::Ins().GetCodecInfo(m_nCodeId, inst);
        PRTP_HEADER pRTPHeader = (PRTP_HEADER) pRTPPacket;
        memset(pRTPHeader, 0, RTP_HEADER_LEN);
        pRTPHeader->version = RTP_VERSION;
        pRTPHeader->type = inst.pltype;
        pRTPHeader->marker = AUDEC_HEADER_GET_SYN_POINT(pData);
        pRTPHeader->timestamp = htonl(m_nRtpTimestamp);
        pRTPHeader->ssrc = htonl((unsigned long) this);
        pRTPHeader->sequence = htons(++m_nRtpSeqNo);
        m_nRtpTimestamp += inst.pacsize;

        memcpy(pRTPPacket + RTP_HEADER_LEN, pData, nLen);
        return nLen + RTP_HEADER_LEN;
    }
}

