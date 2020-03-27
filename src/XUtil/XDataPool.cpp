// XDataPool.cpp: implementation of the XDataPool class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "XDataPool.h"
#include "XAutoLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace VOICEENGINEAPI {
    XDataPool::XDataPool(int32_t nMargin)
            : m_nMargin(nMargin) {
        if (m_nMargin < 0) {
            m_nMargin = 0;
        }
    }

    XDataPool::~XDataPool(void) {
        FlushPool();
    }

    void XDataPool::PushBack(void *pData, int32_t nLen) {
        if (pData == NULL || nLen <= 0)
            return;

        XAutoLock l(m_csListXDataBuffer);
        XDataBuffer *pXDataBuffer = new XDataBuffer;
        if (pXDataBuffer) {
            pXDataBuffer->pBuffer = malloc(nLen + m_nMargin);
            if (pXDataBuffer->pBuffer) {
                memcpy(pXDataBuffer->pBuffer, pData, nLen);
                pXDataBuffer->nLen = nLen;
                m_ListXDataBuffer.push_back(pXDataBuffer);
                return;
            }
            pXDataBuffer->Release();
            pXDataBuffer = NULL;
        }
    }

    void XDataPool::LockPool(void) {
        m_csListXDataBuffer.Lock();
    }

    void XDataPool::UnlockPool(void) {
        m_csListXDataBuffer.UnLock();
    }

    bool XDataPool::get_front(void **ppData, int32_t &nLen) {
        if (m_ListXDataBuffer.size()) {
            XDataBuffer *pXDataBuffer = (XDataBuffer *) m_ListXDataBuffer.front();
            *ppData = pXDataBuffer->pBuffer;
            nLen = pXDataBuffer->nLen;
            return true;
        }
        return false;
    }

    bool XDataPool::get_back(void **ppData, int32_t &nLen) {
        if (m_ListXDataBuffer.size()) {
            XDataBuffer *pXDataBuffer = (XDataBuffer *) m_ListXDataBuffer.back();
            *ppData = pXDataBuffer->pBuffer;
            nLen = pXDataBuffer->nLen;
            return true;
        }
        return false;
    }

    XDataPool::XDataBuffer *XDataPool::GetDataBuffer(void) {
        XAutoLock l(m_csListXDataBuffer);
        if (m_ListXDataBuffer.size()) {
            XDataBuffer *pXDataBuffer = (XDataBuffer *) m_ListXDataBuffer.front();
            m_ListXDataBuffer.pop_front();
            return pXDataBuffer;
        }
        return NULL;
    }

    void XDataPool::PopFront(void) {
        XAutoLock l(m_csListXDataBuffer);
        if (m_ListXDataBuffer.size()) {
            XDataBuffer *pXDataBuffer = (XDataBuffer *) m_ListXDataBuffer.front();
            m_ListXDataBuffer.pop_front();
            pXDataBuffer->Release();
            pXDataBuffer = NULL;
        }
    }

    int32_t XDataPool::GetDataCount(void) {
        XAutoLock l(m_csListXDataBuffer);
        return (m_ListXDataBuffer.size());
    }

    void XDataPool::FlushPool(void) {
        XAutoLock l(m_csListXDataBuffer);
        while (m_ListXDataBuffer.size() > 0) {
            XDataBuffer *pXDataBuffer = (XDataBuffer *) m_ListXDataBuffer.front();
            m_ListXDataBuffer.pop_front();
            pXDataBuffer->Release();
            pXDataBuffer = NULL;
        }
    }
}
