//
// Created by zsy on 2019/5/21.
//

#ifndef NATIVE_AUDIO_OPERATIONMGR_H
#define NATIVE_AUDIO_OPERATIONMGR_H

#include "XThreadBase.h"
#include <list>
#include <mutex>
#include <condition_variable>
#include <stdint.h>
#include "NonCopyable.hpp"

namespace VOICEENGINEAPI {
    class COperationMgr : public XThreadBase, public NonCopyable {
    public:
        COperationMgr();

        ~COperationMgr();

        static COperationMgr& Ins();

        int32_t StartUp();

        void ShutDown();

        void StartAsynOperation(std::function<void()> operation);

        void StartSyncOperation(std::function<void()> operation);

        void ThreadProcMain(void);

    private:
        bool m_bThreadRunning;
        std::list <std::function<void()>> m_lsOperations;
        std::condition_variable m_CV; // 条件变量
        std::condition_variable m_SyncCV;
        std::mutex m_mutexOperation;
        std::mutex m_cvMutex;
        std::mutex m_SyncMutex;
    };
}

#endif //NATIVE_AUDIO_OPERATIONMGR_H
