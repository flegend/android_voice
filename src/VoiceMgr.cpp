//
// Created by zsy on 2019/5/20.
//
#include "VoiceMgr.h"
#include "IVoiceCodec.h"
#include "XUtil/XAutoLock.h"
#include "native_debug.h"

#define MAX_PLAY_NUM 32

namespace VOICEENGINEAPI {

    CVoiceMgr::CVoiceMgr() :
            m_pRecordChannel(NULL) {
        m_listAudioPlayers.clear();
    }

    CVoiceMgr::~CVoiceMgr() {
    }

    CVoiceMgr &CVoiceMgr::Ins() {
        static CVoiceMgr voiceMgr;
        return voiceMgr;
    }

    int32_t CVoiceMgr::VoiceInit() {
        LOGD("CVoiceMgr VoiceInit");
        m_AudioEngine.CreateEngine();
        m_pRecordChannel = new CRecordChannel(&m_AudioEngine);
        m_pRecordChannel->ChannelInit(CHANNEL_RECORD);
        return 0;
    }

    int32_t CVoiceMgr::VoiceUnint() {
        LOGD("CVoiceMgr VoiceUnint");
        if (NULL != m_pRecordChannel) {
            delete m_pRecordChannel;
            m_pRecordChannel = NULL;
        }

        {
            XAutoLock l(m_csPlayer);
            ITER_LIST_PLAYERS it = m_listAudioPlayers.begin();
            while (it != m_listAudioPlayers.end()) {
                CPlayerChannel *player = *it;
                if (NULL != player) {
                    player->StopPlayout();
                    delete player;
                    player = NULL;
                }
                m_listAudioPlayers.erase(it++);
            }
            m_listAudioPlayers.clear();
        }

        m_AudioEngine.ReleaseEngine();
        return 0;
    }

    int32_t CVoiceMgr::SetSendCodec(VOICE_ENGINE_CODEC_ID codec_id) {
        LOGD("CVoiceMgr SetSendCodec codec_id = %d", codec_id);
        if (NULL != m_pRecordChannel) {
            m_pRecordChannel->SetSendCodec(codec_id);
        }
        return 0;
    }

    int32_t CVoiceMgr::GetCodecInfo(VOICE_ENGINE_CODEC_ID codec_id, CodecInst &inst) {
        switch (codec_id) {
            case CODEC_G7221_24K : {
                strcpy(inst.plname, "g7221_24");
                inst.rate = 24000;
                inst.channels = 1;
                inst.plfreq = 16000;
                inst.pltype = G7221_24;
                break;
            }
            case CODEC_G7221_32K : {
                strcpy(inst.plname, "g7221_32");
                inst.rate = 32000;
                inst.channels = 1;
                inst.plfreq = 16000;
                inst.pltype = G7221_32;
                break;
            }
            case CODEC_G7221C_24K : {
                strcpy(inst.plname, "g7221C_24");
                inst.rate = 24000;
                inst.channels = 1;
                inst.plfreq = 32000;
                inst.pltype = G7221C_24;
                break;
            }
            case CODEC_G7221C_32K : {
                strcpy(inst.plname, "g7221C_32");
                inst.rate = 32000;
                inst.channels = 1;
                inst.plfreq = 32000;
                inst.pltype = G7221C_32;
                break;
            }
            case CODEC_G7221C_48K : {
                strcpy(inst.plname, "g7221C_48");
                inst.rate = 48000;
                inst.channels = 1;
                inst.plfreq = 32000;
                inst.pltype = G7221C_48;
                break;
            }
            default: {
                strcpy(inst.plname, "invalid");
                inst.rate = 0;
                inst.channels = 0;
                inst.plfreq = 0;
                inst.pltype = UNKNOWN;
                break;
            }
        }
        inst.pacsize = inst.plfreq / 50;
        return 0;
    }

    VOICE_ENGINE_CODEC_ID CVoiceMgr::GetCodecId(int pltype) {
        VOICE_ENGINE_CODEC_ID codec_id = CODEC_G7221_32K;
        switch (pltype) {
            case G7221_24:
                codec_id = CODEC_G7221_24K;
                break;
            case G7221_32:
                codec_id = CODEC_G7221_32K;
                break;
            case G7221C_24:
                codec_id = CODEC_G7221C_24K;
                break;
            case G7221C_32:
                codec_id = CODEC_G7221C_32K;
                break;
            case G7221C_48:
                codec_id = CODEC_G7221C_48K;
                break;
            default:
                LOGE("GetCodecId pltype = %d not support", pltype);
                break;
        }
        return codec_id;
    }

    int32_t CVoiceMgr::RegisterAudioCallback(Transport *cb) {
        LOGD("CVoiceMgr RegisterAudioCallback");
        if (NULL != m_pRecordChannel) {
            m_pRecordChannel->RegisterAudioCallback(cb);
        }
        return 0;
    }

    int32_t CVoiceMgr::DeRegisterAudioCallback() {
        LOGD("CVoiceMgr DeRegisterAudioCallback");
        if (NULL != m_pRecordChannel) {
            m_pRecordChannel->DeRegisterAudioCallback();
        }
        return 0;
    }

    int32_t CVoiceMgr::StartRecording() {
        LOGD("CVoiceMgr StartRecording");
        if (NULL != m_pRecordChannel) {
            return m_pRecordChannel->StartRecording();
        }
        return 0;
    }

    int32_t CVoiceMgr::StopRecording() {
        LOGD("CVoiceMgr StopRecording");
        if (NULL != m_pRecordChannel) {
            return m_pRecordChannel->StopRecording();
        }
        return 0;
    }

    int32_t CVoiceMgr::CreateDecodeChannel() {
        XAutoLock l(m_csPlayer);
        if (m_listAudioPlayers.size() == MAX_PLAY_NUM) {
            LOGE("CVoiceMgr CreateDecodeChannel Error, Out of Range(%d)", MAX_PLAY_NUM);
            return -1;
        }

        int32_t findChannelId = 0;
        for (findChannelId = 0; findChannelId < MAX_PLAY_NUM; findChannelId++) {
            bool bFind = true;
            ITER_LIST_PLAYERS it = m_listAudioPlayers.begin();
            while (it != m_listAudioPlayers.end()) {
                CPlayerChannel *player = *it;
                if (NULL != player) {
                    int32_t channelid = player->GetChannelId();
                    if (channelid == findChannelId) {
                        bFind = false;
                        break;
                    }
                }
                ++it;
            }

            if (bFind) break;
        }

        CPlayerChannel *player = new CPlayerChannel(&m_AudioEngine);
        m_listAudioPlayers.push_back(player);

        LOGD("CVoiceMgr CreateDecodeChannel success, channelID = %d, decode_size = %d",
             findChannelId,
             m_listAudioPlayers.size());

        player->ChannelInit(CHANNEL_PLAYER);
        player->SetChannlID(findChannelId);
//        player->SetCodec(codec_id);

        return findChannelId;
    }

    int32_t CVoiceMgr::DestroyDecodeChannel(int32_t channelID) {
        XAutoLock l(m_csPlayer);
        ITER_LIST_PLAYERS it = m_listAudioPlayers.begin();
        while (it != m_listAudioPlayers.end()) {
            CPlayerChannel *player = *it;
            if (NULL != player) {
                if (player->GetChannelId() == channelID) {
                    player->StopPlayout();
                    delete player;
                    player = NULL;
                    m_listAudioPlayers.erase(it);
                    break;
                }
            }
            ++it;
        }

        LOGD("CVoiceMgr DestroyDecodeChannel success, channelID = %d, decode_size = %d", channelID,
             m_listAudioPlayers.size());
        return 0;
    }

    int32_t CVoiceMgr::Decode(int32_t channelID, const int8_t *data, int32_t nlen) {
//        LOGD("CVoiceMgr Decode channelID = %d, nlen = %d", channelID, nlen);
        XAutoLock l(m_csPlayer);
        ITER_LIST_PLAYERS it = m_listAudioPlayers.begin();
        while (it != m_listAudioPlayers.end()) {
            CPlayerChannel *player = *it;
            if (NULL != player) {
                if (player->GetChannelId() == channelID) {
                    player->Decode(data, nlen);
                    break;
                }
            }
            ++it;
        }
        return 0;
    }

    int32_t CVoiceMgr::StartPlayout(int32_t channelID) {
        LOGD("CVoiceMgr StartPlayout channelID = %d", channelID);
        XAutoLock l(m_csPlayer);
        ITER_LIST_PLAYERS it = m_listAudioPlayers.begin();
        while (it != m_listAudioPlayers.end()) {
            CPlayerChannel *player = *it;
            if (NULL != player) {
                if (player->GetChannelId() == channelID) {
                    player->StartPlayout();
                    break;
                }
            }
            ++it;
        }
        return 0;
    }

    int32_t CVoiceMgr::StopPlayout(int32_t channelID) {
        LOGD("CVoiceMgr StopPlayout channelID = %d", channelID);
        XAutoLock l(m_csPlayer);
        ITER_LIST_PLAYERS it = m_listAudioPlayers.begin();
        while (it != m_listAudioPlayers.end()) {
            CPlayerChannel *player = *it;
            if (NULL != player) {
                if (player->GetChannelId() == channelID) {
                    player->StopPlayout();
                    break;
                }
            }
            ++it;
        }
        return 0;
    }

}
