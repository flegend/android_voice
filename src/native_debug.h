//
// Created by root on 19-5-14.
//

#ifndef NATIVE_AUDIO_NATIVE_DEBUG_H
#define NATIVE_AUDIO_NATIVE_DEBUG_H

#include <android/log.h>

#define LOG_TAG "zsy_opensles_audio"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define ASSERT(cond, fmt, ...)                                    \
    if (!(cond)) {                                                \
        __android_log_assert(#cond, LOG_TAG, fmt, ##__VA_ARGS__); \
    }

#endif //NATIVE_AUDIO_NATIVE_DEBUG_H
