// XDataPool.h: interface for the XDataPool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XDATAPOOL_H__
#define __XDATAPOOL_H__

#include <stdlib.h>

#include "XCritSec.h"
#include "XListPtr.h"

namespace VOICEENGINEAPI {
    class XDataPool {
    public:
        XDataPool(int32_t nMargin = 0);

        virtual ~XDataPool(void);

    public:
        virtual void PushBack(void *pData, int32_t nLen);

        virtual void LockPool(void);

        virtual void UnlockPool(void);

        virtual bool get_front(void **ppData, int32_t &nLen);

        virtual bool get_back(void **ppData, int32_t &nLen);

        virtual void PopFront(void);

        virtual int32_t GetDataCount(void);

        virtual void FlushPool(void);

    public:
        class XDataBuffer {
        public:
            XDataBuffer(void)
                    : pBuffer(NULL), nLen(0) {
            }

            virtual~XDataBuffer(void) {
                if (pBuffer) {
                    free(pBuffer);
                    pBuffer = NULL;
                }
            }

        public:
            void Release(void) {
                if (pBuffer) {
                    free(pBuffer);
                    pBuffer = NULL;
                }
                delete this;
            }

            void *pBuffer;
            int32_t nLen;
        };

        virtual XDataPool::XDataBuffer *GetDataBuffer(void);

    protected:
        XListPtr m_ListXDataBuffer;
        XCritSec m_csListXDataBuffer;
        int32_t m_nMargin;
    };
}

#endif