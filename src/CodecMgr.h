//
// Created by zsy on 2019/5/21.
//

#ifndef NATIVE_AUDIO_CODECMGR_H
#define NATIVE_AUDIO_CODECMGR_H

#include "IVoiceCodec.h"

namespace VOICEENGINEAPI {
    class CodecMgr {
    public:
        CodecMgr();

        virtual ~CodecMgr();

        static CodecMgr &Ins();

        IVoiceCodec *CreateCodec(CodecType codecType);

        void DestroyCodec(IVoiceCodec *codec);
    };
}

#endif //NATIVE_AUDIO_CODECMGR_H
