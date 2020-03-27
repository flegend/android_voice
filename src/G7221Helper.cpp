//
// Created by root on 19-5-21.
//

#include "G7221Helper.h"


namespace VOICEENGINEAPI {
    CG7221Helper::CG7221Helper() :
            m_g7221_encode_state(NULL),
            m_g7221_decode_state(NULL) {

    }

    CG7221Helper::~CG7221Helper() {
        EncodeUnint();
        DecodeUnint();
    }

    bool
    CG7221Helper::DecodeInit(int32_t bit_rate, int32_t sample_rate) {
        bool ret = false;
        if (NULL == m_g7221_decode_state) {
            m_g7221_decode_state = g722_1_decode_init(m_g7221_decode_state, bit_rate, sample_rate);
        }

        if (NULL != m_g7221_decode_state) {
            ret = true;
        }
        return ret;
    }

    bool CG7221Helper::DecodeUnint() {
        if (NULL != m_g7221_decode_state) {
            g722_1_decode_release(m_g7221_decode_state);
            m_g7221_decode_state = NULL;
        }
        return true;
    }

    bool CG7221Helper::DecodeAudio(const int8_t *inputData, int32_t inputLen, int8_t *outputData,
                                   int32_t &outputLen) {
        if (NULL != m_g7221_decode_state) {
            int32_t len = g722_1_decode(m_g7221_decode_state, (int16_t *) outputData,
                                        (const uint8_t *) inputData,
                                        inputLen);
            if (len <= 0) {
                return false;
            }

            outputLen = len * 2;
        }
        return true;
    }

    bool
    CG7221Helper::EncodeInit(int32_t bit_rate, int32_t sample_rate) {
        bool ret = false;
        if (NULL == m_g7221_encode_state) {
            m_g7221_encode_state = g722_1_encode_init(m_g7221_encode_state, bit_rate, sample_rate);
        }

        if (NULL != m_g7221_encode_state) {
            ret = true;
        }
        return ret;
    }

    bool CG7221Helper::EncodeUnint() {
        if (NULL != m_g7221_encode_state) {
            g722_1_encode_release(m_g7221_encode_state);
            m_g7221_encode_state = NULL;
        }
        return true;
    }

    bool CG7221Helper::EncodeAudio(const int8_t *inputData, int32_t inputLen, int8_t *outputData,
                                   int32_t &outputLen) {
        if (NULL != m_g7221_encode_state) {
            int32_t len = g722_1_encode(m_g7221_encode_state, (uint8_t *) outputData,
                                        (int16_t *) inputData,
                                        inputLen / 2);
            if (len <= 0) {
                return false;
            }

            outputLen = len;
        }
        return true;
    }
}