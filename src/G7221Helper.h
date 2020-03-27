//
// Created by root on 19-5-21.
//

#ifndef NATIVE_AUDIO_G7221HELPER_H
#define NATIVE_AUDIO_G7221HELPER_H

#include "libg7221/g722_1/g722_1.h"
#include "IVoiceCodec.h"

namespace VOICEENGINEAPI {
    class CG7221Helper : public IVoiceCodec {
    public:
        CG7221Helper();

        ~CG7221Helper();

    public:
        virtual bool DecodeInit(int32_t bit_rate, int32_t sample_rate);

        virtual bool DecodeUnint();

        virtual bool DecodeAudio(const int8_t *inputData, int32_t inputLen, int8_t *outputData,
                                 int32_t &outputLen);

        virtual bool EncodeInit(int32_t bit_rate, int32_t sample_rate);

        virtual bool EncodeUnint();

        virtual bool EncodeAudio(const int8_t *inputData, int32_t inputLen, int8_t *outputData,
                                 int32_t &outputLen);

    private:
        g722_1_decode_state_t *m_g7221_decode_state;
        g722_1_encode_state_t *m_g7221_encode_state;
    };
}

#endif //NATIVE_AUDIO_G7221HELPER_H
