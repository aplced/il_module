#ifndef __CComponentDispatcherEmptyThisBufferHandle__H__
#define __CComponentDispatcherEmptyThisBufferHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherEmptyThisBufferHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherEmptyThisBufferHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif