// XThreadBase.cpp: implementation of the XThreadBase class.
//
//////////////////////////////////////////////////////////////////////
#include "XThreadBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace VOICEENGINEAPI {
#ifdef WIN32
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop) 
#endif

    XThreadBase::XThreadBase() {
#ifndef WIN32
        sem_init(&m_semWaitForStop, 0, 1);
        m_thread = (pthread_t) -1;
        m_bThreadStopped = true;
#else
        m_dwThreadID = 0;
        m_hThread = NULL;
        m_evStop = CreateEvent(NULL, TRUE, TRUE, NULL);
        SetEvent(m_evStop);
#endif
    }

    XThreadBase::~XThreadBase() {
#ifndef WIN32
        sem_destroy(&m_semWaitForStop);
#else
        CloseHandle(m_evStop);
#endif
    }

    bool XThreadBase::StartThread() {
#ifndef WIN32
        if (pthread_create(&m_thread, NULL, InitThreadProc, (void *) this) != 0)
            return false;
        sem_wait(&m_semWaitForStop);
        m_bThreadStopped = false;
        return true;
#else
        //如果线程已经创建，则不需要再创建
        if (!m_hThread){
            //创建并启动线程函数
            m_hThread = CreateThread(
                NULL,            //the handle cannot be inherited. 
                0,                //the default Thread Stack Size is set
                InitThreadProc,    //线程函数
                this,            //线程函数的参数
                0,                //使线程函数创建完后立即启动
                &m_dwThreadID    //receives the thread identifier
                );
        }//end if (!m_hThread...

        if (m_hThread)
            ResetEvent(m_evStop);

        SetThreadPriority(m_hThread, THREAD_PRIORITY_HIGHEST);

        return m_hThread != NULL;
#endif
    }

    bool XThreadBase::StartThreadNew() {
#ifndef WIN32
        if (pthread_create(&m_thread, NULL, InitThreadProc, (void *) this) != 0)
            return false;
        sem_wait(&m_semWaitForStop);
        m_bThreadStopped = false;
        return true;
#else
        //如果线程已经创建，则不需要再创建
        if (!m_hThread){
            //创建并启动线程函数
            m_hThread = CreateThread(
                NULL,            //the handle cannot be inherited. 
                0,                //the default Thread Stack Size is set
                InitThreadProc,    //线程函数
                this,            //线程函数的参数
                0,                //使线程函数创建完后立即启动
                &m_dwThreadID    //receives the thread identifier
                );
        }//end if (!m_hThread...

        if (m_hThread)
            ResetEvent(m_evStop);

        SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL);

        return m_hThread != NULL;
#endif
    }

    void XThreadBase::WaitForStop() {
#ifndef WIN32
        if (m_bThreadStopped == false) {
            sem_wait(&m_semWaitForStop);
            m_bThreadStopped = true;
            pthread_join(m_thread, NULL);
            int value = -2;
            sem_getvalue(&m_semWaitForStop, &value);
            if (value == 0) {
                sem_post(&m_semWaitForStop);
            }
        }
#else

        WaitForSingleObject(m_evStop, INFINITE);

        if (m_hThread)
        {
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }
#endif
    }

    void XThreadBase::SetName(const char *name) {
#ifdef WIN32
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = m_dwThreadID;
        info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
        __try{
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER){
        }
#pragma warning(pop)  
#else
#ifdef __APPLE__
        // to do
#else
        prctl(PR_SET_NAME, name);
        //pthread_setname_np(m_thread, name);
#endif
#endif // WIN32
    }

    unsigned long XThreadBase::ThreadProc() {
        ThreadProcMain();
#ifndef WIN32
        sem_post(&m_semWaitForStop);
#else
        SetEvent(m_evStop);
#endif
        return 0;
    }
}
