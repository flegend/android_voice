//
// Created by zsy on 2019/5/20.
//

#ifndef NATIVE_AUDIO_IVOICECHANNEL_H
#define NATIVE_AUDIO_IVOICECHANNEL_H

#include "XUtil/XThreadBase.h"

namespace VOICEENGINEAPI {

    typedef enum {
        CHANNEL_RECORD,
        CHANNEL_PLAYER,
        CHANNEL_OTHER,
    } ChannelType;

    class IVoiceChannel : public XThreadBase {
    public:
        virtual void ChannelInit(ChannelType type) = 0;

        virtual void ThreadProcMain(void) = 0;
    };
}


#endif //NATIVE_AUDIO_IVOICECHANNEL_H
