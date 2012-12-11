#ifndef __CComponentDispatcherFillThisBufferHandle__H__
#define __CComponentDispatcherFillThisBufferHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherFillThisBufferHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherFillThisBufferHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif