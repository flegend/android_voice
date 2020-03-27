//
// Created by zsy on 2019/5/21.
//
#include "CodecMgr.h"
#include "G7221Helper.h"
#include "native_debug.h"

namespace VOICEENGINEAPI {
    CodecMgr::CodecMgr() {
    }

    CodecMgr::~CodecMgr() {

    }

    CodecMgr &CodecMgr::Ins() {
        static CodecMgr mgr;
        return mgr;
    }

    IVoiceCodec *CodecMgr::CreateCodec(CodecType codecType) {
        IVoiceCodec *codec = NULL;
        if (codecType >= G7221_24 && codecType <= G7221C_48) {
            codec = new CG7221Helper();
        }
        if (NULL == codec) {
            LOGE("CodecMgr CreateCodec can not find codecType(%d)", codecType);
        }
        return codec;
    }

    void CodecMgr::DestroyCodec(IVoiceCodec *codec) {
        if (NULL != codec) {
            delete codec;
            codec = NULL;
        }
    }
}