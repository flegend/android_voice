//
// Created by zsy on 2019/5/20.
//

#ifndef NATIVE_AUDIO_PLAYERCHANNEL_H
#define NATIVE_AUDIO_PLAYERCHANNEL_H

#include "IVoiceChannel.h"
#include "OpenSLESCore.h"
#include "IVoiceCodec.h"
#include "XUtil/XDataPool.h"
#include <stdint.h>

namespace VOICEENGINEAPI {
    class CPlayerChannel : public IVoiceChannel {
    public:
        CPlayerChannel(CAudioEngine *engine);

        ~CPlayerChannel();

    public:
        virtual void ChannelInit(ChannelType type);

        virtual void ThreadProcMain(void);

        void SetChannlID(int32_t nChannelId);

        int32_t GetChannelId();

        int32_t SetCodec(VOICE_ENGINE_CODEC_ID codec_id);

        int32_t Decode(const int8_t *data, int32_t nlen);

        int32_t StartPlayout();

        int32_t StopPlayout();

    private:
        bool CloseCodec();

        bool InitDecode(int32_t bit_rate, int32_t sample_rate);

        void DoDecode(const int8_t *pData, int32_t nLen);

        void OnDecodeData(const int8_t *pData, int32_t nLen);

        int32_t CreatePlayer();

        int32_t ReleasePlayer();

    private:
        CAudioEngine *m_pAudioEngine;
        CAudioPlayer *m_pAudioPlayer;
        IVoiceCodec *m_pCodec;
        VOICE_ENGINE_CODEC_ID m_nCodeId;
        ChannelType m_nChannelType;
        XDataPool m_AudioDataPool;
        bool m_bPlaying;
        int32_t m_nChannelId;

        XCritSec m_csPlayer;
    };
}
#endif //NATIVE_AUDIO_PLAYERCHANNEL_H
