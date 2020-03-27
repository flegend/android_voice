// AVUtil.h: interface for the AVUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XUTIL_H__
#define __XUTIL_H__

#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>

namespace VOICEENGINEAPI {
    uint32_t XGetTimestamp(void);

    void XSleep(uint32_t ulMS);

    std::string XInt2Str(int32_t nValue);

    int32_t XStr2Int(const std::string &strValue);
}


#endif
