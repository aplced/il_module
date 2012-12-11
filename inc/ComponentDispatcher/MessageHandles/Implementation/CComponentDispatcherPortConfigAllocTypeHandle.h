#ifndef __CComponentDispatcherPortConfigAllocTypeHandle__H__
#define __CComponentDispatcherPortConfigAllocTypeHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherPortConfigAllocTypeHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherPortConfigAllocTypeHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif