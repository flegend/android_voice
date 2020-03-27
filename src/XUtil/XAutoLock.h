// XAutoLock.h: interface for the AutoLock class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XAUTOLOCK_H__
#define __XAUTOLOCK_H__

#include "XCritSec.h"

namespace VOICEENGINEAPI {
    class XAutoLock {
    public:
        XAutoLock(XCritSec &rXCritSec);

        XAutoLock(XCritSec &rXCritSec, int nUs, int nCount, int &nRet);

        ~XAutoLock(void);

    protected:
        XCritSec &m_rXCritSec;
    };
}
#endif 