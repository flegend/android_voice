//
// Created by zsy on 2019/5/21.
//

#ifndef NATIVE_AUDIO_IVOICECODEC_H
#define NATIVE_AUDIO_IVOICECODEC_H

#include <stdio.h>
#include <stdint.h>

namespace VOICEENGINEAPI {
    typedef enum {
        AAC = 0,
        GSM,
        G7221_24 = 121,
        G7221_32,
        G7221C_24,
        G7221C_32,
        G7221C_48,
        UNKNOWN = 9999,
    } CodecType;

    class IVoiceCodec {
    public:
        virtual bool DecodeInit(int32_t bit_rate, int32_t sample_rate) = 0;

        virtual bool DecodeUnint() = 0;

        virtual bool DecodeAudio(const int8_t *inputData, int32_t inputLen, int8_t *outputData,
                                 int32_t &outputLen) = 0;

        virtual bool EncodeInit(int32_t bit_rate, int32_t sample_rate) = 0;

        virtual bool EncodeUnint() = 0;

        virtual bool EncodeAudio(const int8_t *inputData, int32_t inputLen, int8_t *outputData,
                                 int32_t &outputLen) = 0;
    };
}

#endif //NATIVE_AUDIO_IVOICECODEC_H
