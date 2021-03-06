#ifndef __CComponentDispatcherDVPKernelNodeFreeHandle__H__
#define __CComponentDispatcherDVPKernelNodeFreeHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPKernelNodeFreeHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPKernelNodeFreeHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif