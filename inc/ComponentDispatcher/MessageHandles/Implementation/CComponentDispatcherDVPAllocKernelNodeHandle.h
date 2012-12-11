#ifndef __CComponentDispatcherDVPAllocKernelNodeHandle__H__
#define __CComponentDispatcherDVPAllocKernelNodeHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPAllocKernelNodeHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPAllocKernelNodeHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif