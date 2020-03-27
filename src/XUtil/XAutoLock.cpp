// AutoLock.cpp: implementation of the AutoLock class.
//
//////////////////////////////////////////////////////////////////////

#include "XAutoLock.h"
#include "XUtil.h"

#define MAX_TRY_COUNT (int)(100)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace VOICEENGINEAPI {
    XAutoLock::XAutoLock(XCritSec &rXCritSec)
            : m_rXCritSec(rXCritSec) {
        m_rXCritSec.Lock();
    }

    XAutoLock::XAutoLock(XCritSec &rXCritSec, int nUs, int nCount, int &nRet)
            : m_rXCritSec(rXCritSec) {
        nRet = -1;

        if ((nCount <= 0) || (nCount > MAX_TRY_COUNT)) {
            return;
        }

        for (int i = 0; i < nCount; i++) {
            nRet = m_rXCritSec.TryLock();
            if (0 == nRet) {
                // Got the lock
                return;
            } else {
                // Sleep and try again
                XSleep(nUs);
            }
        }

    }

    XAutoLock::~XAutoLock() {
        m_rXCritSec.UnLock();
    }
}
