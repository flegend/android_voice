//
// Created by zsy on 2019/5/21.
//
#include "XUtil.h"
#include <stdlib.h>

namespace VOICEENGINEAPI {
    uint32_t XGetTimestamp(void) {
#ifdef WIN32
        return ::timeGetTime();//毫秒
#else
        struct timeval now;
        gettimeofday(&now, NULL);
        return now.tv_sec * 1000 + now.tv_usec / 1000;
#endif
    }

    void XSleep(uint32_t ulMS) {
#ifdef WIN32
        Sleep(ulMS);
#else
        usleep(ulMS * 1000);
#endif
    }

    std::string XInt2Str(int32_t nValue) {
        char tmp[100];
        sprintf(tmp, "%d", nValue);
        return tmp;
    }

    int32_t XStr2Int(const std::string &strValue) {
        return atoi(strValue.c_str());
    }
}

