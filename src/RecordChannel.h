//
// Created by zsy on 2019/5/20.
//

#ifndef NATIVE_AUDIO_RECORDCHANNEL_H
#define NATIVE_AUDIO_RECORDCHANNEL_H

#include <my_voice_engine_types.h>
#include "IVoiceChannel.h"
#include "OpenSLESCore.h"
#include "XUtil/XDataPool.h"
#include "IVoiceCodec.h"

namespace VOICEENGINEAPI {
    class CRecordChannel : public IVoiceChannel, public Transport {
    public:
        CRecordChannel(CAudioEngine *engine);

        ~CRecordChannel();

    public:
        virtual void ChannelInit(ChannelType type);

        virtual void ThreadProcMain(void);

        virtual int SendPacket(int channel, const void *data, size_t len);

        virtual int SendRTCPPacket(int channel, const void *data, size_t len) {}

        int32_t SetSendCodec(VOICE_ENGINE_CODEC_ID codec_id);

        int32_t RegisterAudioCallback(Transport *cb);

        int32_t DeRegisterAudioCallback();

        int32_t StartRecording();

        int32_t StopRecording();

    private:
        bool CloseCodec();

        bool InitEncode(int32_t bit_rate, int32_t sample_rate);

        void DoEncode(const int8_t *pData, int32_t nLen);

        void OnEncodeData(const int8_t *pData, int32_t nLen);

        int32_t AddRTPHeader(uint8_t* pRTPPacket, const uint8_t* pData, int32_t nLen);

    private:
        CAudioEngine *m_pAudioEngine;
        CAudioRecord *m_pAudioRecord;
        Transport *m_pCallBack;
        IVoiceCodec *m_pCodec;
        VOICE_ENGINE_CODEC_ID m_nCodeId;
        ChannelType m_nChannelType;
        XDataPool m_AudioDataPool;
        bool m_bRecording;
        XCritSec m_csCallback;
        uint32_t m_nRtpTimestamp;
        uint32_t m_nRtpSeqNo;
    };
}
#endif //NATIVE_AUDIO_RECORDCHANNEL_H
