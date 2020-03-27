//
// Created by zsy on 2019/5/20.
//

#ifndef NATIVE_AUDIO_VOICEMGR_H
#define NATIVE_AUDIO_VOICEMGR_H

#include <stdint.h>
#include <list>

#include "RecordChannel.h"
#include "PlayerChannel.h"
#include "OpenSLESCore.h"
#include "../include/my_voice_engine_types.h"

namespace VOICEENGINEAPI {

    typedef std::list<CPlayerChannel *> LIST_PLAYERS;
    typedef LIST_PLAYERS::iterator ITER_LIST_PLAYERS;

    class CVoiceMgr {
    public:
        CVoiceMgr();

        ~CVoiceMgr();

        static CVoiceMgr &Ins();

        int32_t VoiceInit();

        int32_t VoiceUnint();

        int32_t GetCodecInfo(VOICE_ENGINE_CODEC_ID codec_id, CodecInst &inst);

        VOICE_ENGINE_CODEC_ID GetCodecId(int pltype);

        int32_t SetSendCodec(VOICE_ENGINE_CODEC_ID codec_id);

        int32_t RegisterAudioCallback(Transport *cb);

        int32_t DeRegisterAudioCallback();

        int32_t StartRecording();

        int32_t StopRecording();

        int32_t CreateDecodeChannel();

        int32_t DestroyDecodeChannel(int32_t decodeChannelID);

        int32_t Decode(int32_t channelID, const int8_t *data, int32_t nlen);

        int32_t StartPlayout(int32_t channelID);

        int32_t StopPlayout(int32_t channelID);

    private:
        CRecordChannel *m_pRecordChannel;

        CAudioEngine m_AudioEngine;

        LIST_PLAYERS m_listAudioPlayers;

        XCritSec m_csPlayer;
    };
}

#endif //NATIVE_AUDIO_VOICEMGR_H
