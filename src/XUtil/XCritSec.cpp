// XCritSec.cpp: implementation of the XCritSec class.
//
//////////////////////////////////////////////////////////////////////
#include "XCritSec.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace VOICEENGINEAPI {
    XCritSec::XCritSec(void) {
#ifdef WIN32
        InitializeCriticalSection(&m_CritSec);
#else
#if(0)
        //linux 7.3
        pthread_mutex_init(&m_pMutex, NULL);
#else
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
#ifndef __APPLE__
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_pMutex, &attr);
        pthread_mutexattr_destroy(&attr);
#endif
#endif
    }

    XCritSec::~XCritSec(void) {
#ifdef WIN32
        DeleteCriticalSection(&m_CritSec);
#else
        pthread_mutex_destroy(&m_pMutex);
#endif
    }

    void XCritSec::Lock(void) {
#ifdef WIN32
        EnterCriticalSection(&m_CritSec);
#else
        (void) pthread_mutex_lock(&m_pMutex);
#endif
    }

    int XCritSec::TryLock(void) {
        int ret = 0;
#ifdef WIN32
        //TODO
#else
        ret = pthread_mutex_trylock(&m_pMutex);
#endif
        return ret;
    }


    void XCritSec::UnLock(void) {
#ifdef WIN32
        LeaveCriticalSection(&m_CritSec);
#else
        pthread_mutex_unlock(&m_pMutex);
#endif
    }
}
