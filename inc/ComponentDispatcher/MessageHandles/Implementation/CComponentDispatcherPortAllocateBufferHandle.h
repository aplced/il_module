#ifndef __CComponentDispatcherPortAllocateBufferHandle__H__
#define __CComponentDispatcherPortAllocateBufferHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherPortAllocateBufferHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherPortAllocateBufferHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif