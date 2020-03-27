//
// Created by zsy on 2019/5/21.
//
#include "OperationMgr.h"
#include "../native_debug.h"

namespace VOICEENGINEAPI {
    COperationMgr::COperationMgr()
            : m_bThreadRunning(false) {
    }

    COperationMgr::~COperationMgr() {
        ShutDown();
    }

    int32_t COperationMgr::StartUp() {
        if (m_bThreadRunning)
            return -1;

        m_bThreadRunning = true;
        if (!XThreadBase::StartThread()) {
            m_bThreadRunning = false;
            return -1;
        }
        SetName("android_voice");
        return 0;
    }

    void COperationMgr::ShutDown() {
        m_bThreadRunning = false;
        m_CV.notify_one();
        XThreadBase::WaitForStop();
        m_lsOperations.clear();
    }

    void COperationMgr::StartAsynOperation(std::function<void()> operation) {
        std::lock_guard<std::mutex> lock(m_mutexOperation);
        m_lsOperations.push_back(operation);
        m_CV.notify_one();
    }

    void COperationMgr::StartSyncOperation(std::function<void()> operation) {
        std::unique_lock<std::mutex> lck(m_cvMutex);
        StartAsynOperation([&] {
                               operation();
                               m_SyncCV.notify_one();
                           }
        );
        m_SyncCV.wait(lck);
    }

    COperationMgr &COperationMgr::Ins() {
        static COperationMgr mgr;
        return mgr;
    }

    void COperationMgr::ThreadProcMain(void) {
        std::unique_lock<std::mutex> lck(m_cvMutex);
        while (m_bThreadRunning) {
            if (!m_bThreadRunning)
                break;

            m_mutexOperation.lock();
            if (m_lsOperations.empty()) {
                m_mutexOperation.unlock();
                m_CV.wait(lck);
                m_mutexOperation.lock();
            }

            if (!m_bThreadRunning) {
                m_mutexOperation.unlock();
                break;
            }

            std::function<void()> fun = m_lsOperations.front();
            m_lsOperations.pop_front();
            m_mutexOperation.unlock();

            if (!m_bThreadRunning)
                break;

//            LOGD("function processing------%s--------begin,current pool size:%d", fun.target_type().name(), m_lsOperations.size());
            fun();
//            LOGD("function processing------%s--------end", fun.target_type().name());
        }
    }
}
